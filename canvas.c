#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>

#define W 32
#define H 32

const char *vertex_shader =
  "#version 330\n"
  "layout (location = 0) in vec3 vp;"
  "layout (location = 1) in vec2 vt;"
  "out vec2 texture_coordinates;"
  "void main () {"
  "  texture_coordinates = vt;"
  "  gl_Position = vec4(vp, 1.0);"
  "}";

const char *fragment_shader =
  "#version 330\n"
  "in vec2 texture_coordinates;"
  "uniform sampler2D basic_texture;"
  "out vec4 frag_color;"
  "void main () {"
  "  vec4 texel = texture (basic_texture, texture_coordinates);"
  "  frag_color = texel;"
  "}";

typedef struct {
  GLbyte *buf;
  int w;
  int h;
} frame;

typedef struct {
  int i, j;
} vec2i;

float pal[16][4] = {
  {0.53f, 0.00f, 0.00f, 1.0f},
  {0.07f, 0.13f, 0.27f, 1.0f},
  {0.07f, 0.20f, 0.33f, 1.0f},
  {0.13f, 0.27f, 0.40f, 1.0f},
  {0.20f, 0.33f, 0.47f, 1.0f},
  {0.33f, 0.47f, 0.60f, 1.0f},
  {0.47f, 0.67f, 0.73f, 1.0f},
  {0.73f, 0.53f, 0.00f, 1.0f},
  {1.00f, 0.00f, 0.00f, 1.0f},
  {0.80f, 0.60f, 0.00f, 1.0f},
  {0.87f, 0.67f, 0.00f, 1.0f},
  {0.93f, 0.80f, 0.00f, 1.0f},
  {1.00f, 0.93f, 0.00f, 1.0f},
  {1.00f, 1.00f, 0.47f, 1.0f},
  {1.00f, 1.00f, 0.67f, 1.0f}
};

void draw_point(frame *f, int i, int j, int color) {
  for (int k=0; k<4; k++)
    f->buf[k+4*f->w*j+4*i] = (GLbyte)(pal[color % 16][k] * 255.0f);
}

void draw_rect(frame *f, int x0, int y0, int x1, int y1, int color) {
  for (int x=x0; x<x1; x++)
    for (int y=y0; y<y1; y++)
      draw_point(f, x, y, color);
}

void draw_line(frame *f, int x0, int y0, int x1, int y1, int color) {

  printf("draw_line x0=%d, y0=%d, x1=%d, y1=%d, color=%d\n", x0, y0, x1, y1, color);

  int xa = x0<x1?x0:x1;
  int ya = y0<y1?y0:y1;
  int xb = x0<x1?x1:x0;
  int yb = y0<y1?y1:y0;

  /* end points */
  draw_point(f, x0, y0, color);
  draw_point(f, x1, y1, color);

  /* horizontal */
  if (x0 == x1) {
    for (int y=ya; y<yb; y++) draw_point(f, x0, y, color);
    return;
  }

  /* vertical */
  if (y0 == y1) {
    for (int x=xa; x<xb; x++) draw_point(f, x, y0, color);
    return;
  }

  int dx = abs(x1-x0);
  int dy = abs(y1-y0);

  if (dy>dx) {
    float sx = (float)(x1-x0)/(float)dy;
    for (int yi=0; yi<dy; yi++) {
      int x=x0+roundf(yi*sx);
      int sign=(y1-y0)>0?1:-1;
      int y=y0+yi*sign;
      draw_point(f, x, y, color);
    }
    return;
  }
  else {
    float sy = (float)(y1-y0)/(float)dx;
    for (int xi=0; xi<dx; xi++) {
      int y=y0+roundf(xi*sy);
      int sign=(x1-x0)>0?1:-1;
      int x=x0+xi*sign;
      draw_point(f, x, y, color);
    }
    return;
  }
}

void draw_triangle(frame *f, int x0, int y0, int x1, int y1, int x2, int y2, int color) {

  draw_line(f, x0, y0, x1, y1, color);
  draw_line(f, x1, y1, x2, y2, color);
  draw_line(f, x2, y2, x0, y0, color);

}

void clear(frame *f) {
  draw_rect(f, 0,  0,  f->w, f->h, 15);
}

void test_pattern(frame *f, int p) {

  switch (p) {
    case 0:
      for (int i=0; i<f->w; i++) {
        for (int j=0; j<f->h; j++) {
          int color = (j+i)%16;
          draw_point(f, i, j, color);
        }
      }
      break;
    case 1:
      draw_rect(f, 10, 11, 18, 23, 1);
      draw_line(f, 13,  3, 27, 20, 3);
      draw_line(f, 27, 20, 10, 29, 4);
      draw_line(f, 10, 29,  3, 15, 5);
      draw_line(f,  3, 15, 13,  3, 6);
      break;
    default:
      break;
  }
}

int main(int argc, char **argv) {

  float size = 0.95f;
  float scale = 8.0f;
  int width = W * scale;
  int height = H * scale;

  GLbyte canvas[W*H*4];
  ////
  frame f = (frame){canvas, W, H};
  clear(&f);
  test_pattern(&f, 1);
  ////
  GLFWwindow *window = NULL;
  const GLubyte *renderer;
  const GLubyte *version;
  GLuint vert_shader, frag_shader;
  GLuint shader_prog;

  GLfloat vertices[] = {
   -size,  size, 0.0f,
    size,  size, 0.0f,
    size, -size, 0.0f,
    size, -size, 0.0f,
   -size, -size, 0.0f,
   -size,  size, 0.0f };

  GLfloat texcoords[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f };

  puts("initializing glfw");

  if (!glfwInit()) {
     fprintf(stderr,
      "couldn't initialize glfw3\n");
    return -1;
  }

  printf("opening glfw window (%d x %d, scale=%.1f, canvas %d x %d)\n", width, height, scale, W, H);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "canvas", NULL, NULL);

  if (!window) {
    fprintf(stderr,
      "couldn't initialize GLFWwindow\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  renderer = glGetString(GL_RENDERER);
  version = glGetString(GL_VERSION);

  fprintf(stdout,
    "GL_RENDERER: %s\n"
    "GL_VERSION: %s\n",
    renderer, version);
  GLuint tex_vbo, vbo, vao;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  /* allocate GPU memory for vertices */
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
    18 * sizeof(GLfloat),
    vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &tex_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
  glBufferData(GL_ARRAY_BUFFER,
    12 * sizeof(GLfloat),
    texcoords, GL_STATIC_DRAW);

  /* use the vbo and use 3 float per 'variable' */
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT,
    GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
  glVertexAttribPointer(1, 2, GL_FLOAT,
    GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  GLuint tex;
  glGenTextures( 1, &tex );
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, tex );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, f.w, f.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, f.buf );
  glGenerateMipmap( GL_TEXTURE_2D );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );

  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1,
    &vertex_shader, NULL);
  glCompileShader(vert_shader);

  /* fragment shader */
  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1,
    &fragment_shader, NULL);
  glCompileShader(frag_shader);

  /* link glsl program */
  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, frag_shader);
  glAttachShader(shader_prog, vert_shader);
  glLinkProgram(shader_prog);

  /* main loop */
  while (!glfwWindowShouldClose(window)) {
    /* clear */
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_prog);
    glBindVertexArray(vao);

    /* draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwPollEvents();
    if (GLFW_PRESS == glfwGetKey(
      window, GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, 1);
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
