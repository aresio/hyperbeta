#ifndef __COLORS__
#define __COLORS__

// light colors
const GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };  /* White diffuse light. */
const GLfloat light_specular[] = { 1.0, 1.0, 1.0, 0.0 };  /* White specular light. */
const GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };  /* Infinite light location. */
const GLfloat light2_diffuse[] = { 0.75, 0.75, 0.75, 0.0 };  /* White diffuse light. */
const GLfloat light2_specular[] = { 0.75, 0.75, 0.75, 0.0 };  /* White specular light. */
const GLfloat light2_position[] = { -1.0, 0.0, -1.0, 0.0 };  /* Infinite light location. */
const GLfloat white_specular[4] = { 1.f, 1.f, 1.f, 1.f };


// object colors
const glm::vec4* cloud_colors = new glm::vec4(0.0, 1.0, 0.0, 1.0);
const glm::vec4* structure_colors = new glm::vec4(1.0, 0.0, 1.0, 0.6);
const glm::vec4* structure_colors_barely = new glm::vec4(1.0, 0.0, 1.0, 0.35);
const glm::vec4* selected_structure_color = new glm::vec4(1.0, 0.0, 0.0, 0.7);
const glm::vec4* peptide_color = new glm::vec4(0.7f, 0.7f, 0.7f, 0.5);
const glm::vec4* selected_grain_color = new glm::vec4(0.6f, 0.4f, 0.0f, 0.9f);

// skins
glm::vec4* clear_color_dark = new glm::vec4(0, 0, 0, 0);
glm::vec4* clear_color_bright = new glm::vec4(1.f, 1.f, 1.f, 0);

#endif