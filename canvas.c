#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>

#define W 32
#define H 20

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

typedef struct {
  float x,y,w,z;
} vec4f;

void draw_point(frame *f, vec2i *pt, float col[4]) {
  printf("%d, %d = %f,%f,%f,%f\n",
    pt->i, pt->j, col[0], col[1], col[2], col[3]);

  for (int k=0; k<4; k++)
    f->buf[k+4*f->w*pt->j+4*pt->i] = col[k];
}

void test_pattern(frame *f) {

  for (int i=0; i<f->w; i++)
    for (int j=0; j<f->h; j++) {
      float c[4] = {64*((i+j)%4),64*((i-j)%4),32*((i+j)%8),255};
      draw_point(f, &((vec2i){i, j}), c);
    }
}

int main(int argc, char **argv) {

  float size = 0.95f;
  float scale = 8.0f;
  int width = W * scale;
  int height = H * scale;

  GLbyte canvas[W*H*4];
  frame f = (frame){canvas, W, H};
  test_pattern(&f);
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
