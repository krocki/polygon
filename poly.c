#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#if NOGL

// no GL ?
int run() {
  return 0;
}

#else
#include <pthread.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
static GLFWwindow* window;

u8 flip_y=1;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS &&
      key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    printf("scancode = %d, mods = %d\n", scancode, mods);
}

void draw_line(int x0, int y0, int x1, int y1, float r, float g, float b, float a, float w, int scr_w, int scr_h) {

    glLineWidth(w);
    float incr_x = 1.0f/(float)scr_w; float incr_y = 1.0f/(float)scr_h;
    glColor4f(r, g, b, a);
    float i0 = x0 * incr_x;
    float j0 = y0 * incr_y;
    float i1 = x1 * incr_x;
    float j1 = y1 * incr_y;
    j0 = flip_y ? 1-j0 : j0; // FLIP vert
    j1 = flip_y ? 1-j1 : j1; // FLIP vert
    glVertex2f(i0, j0);
    glVertex2f(i1, j1);
}

int run() {

  const GLubyte *renderer;
  const GLubyte *version;

  int gl_width = 320;
  int gl_height = 200;

  if (!glfwInit()) {
    fprintf(stderr,
      "couldn't initialize glfw3\n");
    return -1;
  }

  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(gl_width, gl_height, "Polygon test", NULL, NULL);

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

  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  glEnable(GL_BLEND);

  while (!glfwWindowShouldClose(window)) {

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    //printf("fb size = %d x %d\n", fb_width, fb_height);
    //const float fb_ratio = fb_width / (float) fb_height;
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fb_width, fb_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, gl_width, 0.f, gl_height, 0.f, 1.f);

    glBegin(GL_LINE_STRIP);
    glColor4f(1, 1, 1, 1);
    //float x0 = 0.2f, y0 = 0.2f, x1 = 0.8f, y1 = 0.8f;
    //glVertex2f(x0, y0);
    //glVertex2f(x1, y0);
    //glVertex2f(x1, y1);
    //glVertex2f(x0, y1);
    //glVertex2f(x0, y0);
    int x0 = 1, y0 = 1, x1 = 319,  y1 = 199;
    glVertex2i(x0, y0);
    glVertex2i(x1, y0);
    glVertex2i(x1, y1);
    glVertex2i(x0, y1);
    glVertex2i(x0, y0);
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

#endif

int main(int argc, char **argv) {

  int h = 200, w = 320;
  int buf_size = h * w * sizeof(u8);

  u8 *buf = malloc(buf_size);

  printf("buf: w=%d x h=%d (%d B), ptr=%p\n", w, h, buf_size, buf);

  int res = run();
  printf("res = %d\n", res);
  if (buf) free(buf);
  return 0;
}
