#pragma once

#include <Utilities/Math.h>
#include <Utilities/Logging.h>

namespace pn {

enum class ProjectionType {
	ORTHOGRAPHIC,
	PERSPECTIVE
};

class ProjectionMatrix {
	float				view_width;
	float				view_height;
	float				near_plane;
	float				far_plane;
	float				fov;
	float				orthographic_size;
	ProjectionType		projection_type;

	mutable bool		dirty;
	mutable pn::mat4f	projection_matrix_cache;

public:
	ProjectionMatrix() :
		view_width{ 100.0f }, view_height{ 100.0f }, near_plane{ 0.1f }, far_plane{ 1000.0f },
		dirty{ true }, projection_matrix_cache{}, projection_type{ ProjectionType::PERSPECTIVE },
		fov{ 70.0f }, orthographic_size{ 0.1f } {}

	ProjectionMatrix(ProjectionType projection_type, float view_width, float view_height, float near_plane, float far_plane, float fov, float orthographic_size) :
		view_width{ view_width }, view_height{ view_height }, near_plane{ near_plane }, far_plane{ far_plane },
		dirty{ true }, projection_matrix_cache{}, projection_type{ projection_type },
		fov{ fov }, orthographic_size{ orthographic_size } {}

	float			GetViewWidth() const { return view_width; }
	void			SetViewWidth(float view_width) { this->view_width = view_width; dirty = true; }

	float			GetViewHeight() const { return view_height; }
	void			SetViewHeight(float view_height) { this->view_height = view_height; dirty = true; }

	float			GetNearPlane() const { return near_plane; }
	void			SetNearPlane(float near_plane) { this->near_plane = near_plane; dirty = true; }

	float			GetFarPlane() const { return far_plane; }
	void			SetFarPlane(float far_plane) { this->far_plane = far_plane; dirty = true; }

	float			GetFov() const { return fov; }
	void			SetFov(float fov) { this->fov = fov; dirty = true; }

	float			GetOrthographicSize() const { return orthographic_size; }
	void			SetOrthographicSize(float orthographic_size) { this->orthographic_size = orthographic_size; dirty = true; }

	ProjectionType	GetProjectionType() const { return projection_type; }
	void			SetProjectionType(ProjectionType projection_type) { this->projection_type = projection_type; dirty = true; }

	operator		pn::mat4f() const {
		return GetMatrix();
	}

	pn::mat4f		GetMatrix() const {
		if (dirty) {
			switch (projection_type) {
			case ProjectionType::ORTHOGRAPHIC:
				projection_matrix_cache = Orthographic(orthographic_size * view_width, orthographic_size * view_height, near_plane, far_plane);
				break;
			case ProjectionType::PERSPECTIVE:
				projection_matrix_cache = PerspectiveFov(Rad(fov), view_width / view_height, near_plane, far_plane);
				break;
			default:
				LogError("Unknown projection type");
			}
			dirty = false;
		}
		return projection_matrix_cache;
	}

};

}