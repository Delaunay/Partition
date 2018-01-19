#include <UI/EditorUI.h>

#include <Application/Global.h>

namespace pn {

namespace gui {

// ------- CONSTANTS ---------

const float MAIN_MENU_HEIGHT = 20.0f;

// ------- VARIABLES ----------

bool show_main_menu;
bool show_command_line;

map<int, function_map> functions;
struct AppConsole *    command_line;

// -------- CLASS DEFINITIONS ---------

struct AppConsole {
    char                   InputBuf[256];
    ImVector<char *>       Items;
    bool                   ScrollToBottom;
    ImVector<char *>       History;
    int                    HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char *> Commands;

    AppConsole() {
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
	Commands.push_back("HELP");
	Commands.push_back("HISTORY");
	Commands.push_back("CLEAR");
	Commands.push_back("CLASSIFY"); // "classify" is here to provide an example of "C"+[tab]
	                                // completing to "CL" and displaying matches.
    }
    ~AppConsole() {
	ClearLog();
	for(int i = 0; i < History.Size; i++)
	    free(History[i]);
    }

    // Portable helpers
    static int Stricmp(const char *str1, const char *str2) {
	int d;
	while((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
	    str1++;
	    str2++;
	}
	return d;
    }
    static int Strnicmp(const char *str1, const char *str2, int n) {
	int d = 0;
	while(n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
	    str1++;
	    str2++;
	    n--;
	}
	return d;
    }
    static char *Strdup(const char *str) {
	size_t len  = strlen(str) + 1;
	void * buff = malloc(len);
	return (char *)memcpy(buff, (const void *)str, len);
    }

    void ClearLog() {
	for(int i = 0; i < Items.Size; i++)
	    free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
    }

    void AddLog(const char *fmt, ...) IM_PRINTFARGS(2) {
	char    buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
	ScrollToBottom = true;
    }

    void Draw(const char *title, bool *p_open) {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
	if(!ImGui::Begin(title, p_open)) {
	    ImGui::End();
	    return;
	}

	static ImGuiTextFilter filter;

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()),
	                  false, ImGuiWindowFlags_HorizontalScrollbar);
	if(ImGui::BeginPopupContextWindow()) {
	    if(ImGui::Selectable("Clear"))
		ClearLog();
	    ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for(int i = 0; i < Items.Size; i++) {
	    const char *item = Items[i];
	    if(!filter.PassFilter(item))
		continue;
	    ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // A better implementation may store a type
	                                                 // per-item. For the sample let's just
	                                                 // parse the text.
	    if(strstr(item, "[error]"))
		col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
	    else if(strncmp(item, "# ", 2) == 0)
		col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
	    ImGui::PushStyleColor(ImGuiCol_Text, col);
	    ImGui::TextUnformatted(item);
	    ImGui::PopStyleColor();
	}
	if(ScrollToBottom)
	    ImGui::SetScrollHere();
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	if(ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf),
	                    ImGuiInputTextFlags_EnterReturnsTrue |
	                        ImGuiInputTextFlags_CallbackCompletion |
	                        ImGuiInputTextFlags_CallbackHistory,
	                    &TextEditCallbackStub, (void *)this)) {
	    char *input_end = InputBuf + strlen(InputBuf);
	    while(input_end > InputBuf && input_end[-1] == ' ')
		input_end--;
	    *input_end = 0;
	    if(InputBuf[0])
		ExecCommand(InputBuf);
	    strcpy(InputBuf, "");
	}

	// Demonstrate keeping auto focus on the input box
	if(ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() &&
	                              !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
	    ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
    }

    void ExecCommand(const char *command_line) {
	AddLog("# %s/n", command_line);

	// Insert into history. First find match and delete it so it can be pushed to the back. This
	// isn't trying to be smart or optimal.
	HistoryPos = -1;
	for(int i = History.Size - 1; i >= 0; i--)
	    if(Stricmp(History[i], command_line) == 0) {
		free(History[i]);
		History.erase(History.begin() + i);
		break;
	    }
	History.push_back(Strdup(command_line));

	// parse into command name and args
	auto command = Split(string(command_line), ' ');

	// from name and nargs, choose function
	string command_name = command[0];
	int    nargs        = command.size() - 1;

	if(!Contains(functions, nargs)) {
	    AddLog("Can't execute command: No commands with that number of parameters are "
	           "registered.");
	    return;
	}

	auto &function_map = functions[nargs];
	if(!Contains(function_map, command_name)) {
	    AddLog("Can't execute command: %s with %i parameters not found", command_name.c_str(),
	           nargs);
	    return;
	}

	switch(nargs) {
	case 0: {
	    auto &f =
	        std::any_cast<std::function<StringValue(void)>>(functions[nargs][command_name]);
	    string result = f();
	    AddLog("%s", CString(result));
	} break;
	case 1: {
	    auto &f = std::any_cast<std::function<StringValue(StringValue)>>(
	        functions[nargs][command_name]);
	    string result = f(command[1]);
	    AddLog("%s", CString(result));
	} break;
	case 2: {
	    auto &f = std::any_cast<std::function<StringValue(StringValue, StringValue)>>(
	        functions[nargs][command_name]);
	    string result = f(command[1], command[2]);
	    AddLog("%s", CString(result));
	} break;
	case 3: {
	    auto &f =
	        std::any_cast<std::function<StringValue(StringValue, StringValue, StringValue)>>(
	            functions[nargs][command_name]);
	    string result = f(command[1], command[2], command[3]);
	    AddLog("%s", CString(result));
	} break;
	case 4: {
	    auto &f = std::any_cast<
	        std::function<StringValue(StringValue, StringValue, StringValue, StringValue)>>(
	        functions[nargs][command_name]);
	    string result = f(command[1], command[2], command[3], command[4]);
	    AddLog("%s", CString(result));
	} break;
	case 5: {
	    auto &f = std::any_cast<std::function<StringValue(StringValue, StringValue, StringValue,
	                                                      StringValue, StringValue)>>(
	        functions[nargs][command_name]);
	    string result = f(command[1], command[2], command[3], command[4], command[5]);
	    AddLog("%s", CString(result));
	} break;
	default:
	    AddLog("YOU SHOULDN'T SEE THIS");
	}

	// Process command
	if(Stricmp(command_line, "CLEAR") == 0) {
	    ClearLog();
	} else if(Stricmp(command_line, "HELP") == 0) {
	    AddLog("Commands:");
	    for(int i = 0; i < Commands.Size; i++)
		AddLog("- %s", Commands[i]);
	} else if(Stricmp(command_line, "HISTORY") == 0) {
	    for(int i = History.Size >= 10 ? History.Size - 10 : 0; i < History.Size; i++)
		AddLog("%3d: %s/n", i, History[i]);
	}
    }

    static int TextEditCallbackStub(ImGuiTextEditCallbackData *data) // In C++11 you are better off
                                                                     // using lambdas for this sort
                                                                     // of forwarding callbacks
    {
	AppConsole *console = (AppConsole *)data->UserData;
	return console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiTextEditCallbackData *data) {
	// AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart,
	// data->SelectionEnd);
	switch(data->EventFlag) {
	case ImGuiInputTextFlags_CallbackCompletion: {
	    // Example of TEXT COMPLETION

	    // Locate beginning of current word
	    const char *word_end   = data->Buf + data->CursorPos;
	    const char *word_start = word_end;
	    while(word_start > data->Buf) {
		const char c = word_start[-1];
		if(c == ' ' || c == '/t' || c == ',' || c == ';')
		    break;
		word_start--;
	    }

	    // Build a list of candidates
	    ImVector<const char *> candidates;
	    for(int i = 0; i < Commands.Size; i++)
		if(Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
		    candidates.push_back(Commands[i]);

	    if(candidates.Size == 0) {
		// No match
		AddLog("No match for \"%.*s\"!/n", (int)(word_end - word_start), word_start);
	    } else if(candidates.Size == 1) {
		// Single match. Delete the beginning of the word and replace it entirely so we've
		// got nice casing
		data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
		data->InsertChars(data->CursorPos, candidates[0]);
		data->InsertChars(data->CursorPos, " ");
	    } else {
		// Multiple matches. Complete as much as we can, so inputing "C" will complete to
		// "CL" and display "CLEAR" and "CLASSIFY"
		int match_len = (int)(word_end - word_start);
		for(;;) {
		    int  c                      = 0;
		    bool all_candidates_matches = true;
		    for(int i = 0; i < candidates.Size && all_candidates_matches; i++)
			if(i == 0)
			    c = toupper(candidates[i][match_len]);
			else if(c == 0 || c != toupper(candidates[i][match_len]))
			    all_candidates_matches = false;
		    if(!all_candidates_matches)
			break;
		    match_len++;
		}

		if(match_len > 0) {
		    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
		    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
		}

		// List matches
		AddLog("Possible matches:/n");
		for(int i = 0; i < candidates.Size; i++)
		    AddLog("- %s/n", candidates[i]);
	    }

	    break;
	}
	case ImGuiInputTextFlags_CallbackHistory: {
	    // Example of HISTORY
	    const int prev_history_pos = HistoryPos;
	    if(data->EventKey == ImGuiKey_UpArrow) {
		if(HistoryPos == -1)
		    HistoryPos = History.Size - 1;
		else if(HistoryPos > 0)
		    HistoryPos--;
	    } else if(data->EventKey == ImGuiKey_DownArrow) {
		if(HistoryPos != -1)
		    if(++HistoryPos >= History.Size)
			HistoryPos = -1;
	    }

	    // A better implementation would preserve the data on the current input line along with
	    // cursor position.
	    if(prev_history_pos != HistoryPos) {
		data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen =
		    (int)snprintf(data->Buf, (size_t)data->BufSize, "%s",
		                  (HistoryPos >= 0) ? History[HistoryPos] : "");
		data->BufDirty = true;
	    }
	}
	}
	return 0;
    }
};

// --------- FUNCTIONS --------

void InitEditorUI() {
    show_main_menu    = true;
    show_command_line = false;
    command_line      = new AppConsole;
}

void ShutdownEditorUI() { delete command_line; }

// --------- MAIN MENU ---------

void SetMainMenuVisible(bool value) { show_main_menu = value; }

void DrawMainMenu(const unsigned int screen_width) {
    if(!show_main_menu)
	return;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.0f, 0.0f, 0.0f, 0.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, {0.0f});
    if(ImGui::Begin("Window", NULL,
                    ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
	ImGui::SetWindowPos({0, 0});
	ImGui::SetWindowSize({static_cast<float>(screen_width), MAIN_MENU_HEIGHT});
	if(ImGui::BeginMenuBar()) {
	    if(ImGui::BeginMenu("File")) {

		if(ImGui::MenuItem("Exit", nullptr, nullptr)) {
		    pn::app::Exit();
		}

		ImGui::EndMenu();
	    }
	    ImGui::EndMenuBar();
	}
	ImGui::End();
    }
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

// --------- COMMAND LINE -------

void SetCommandLine(bool value) { show_command_line = value; }

void DrawCommandLine() {
    if(!show_command_line)
	return;
    command_line->Draw("Command Line", nullptr);
}

} // namespace gui

} // namespace pn