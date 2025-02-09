/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "student/fwd.hpp"
#include <cstdint>
#include <student/gpu.hpp>

struct Triangle {
  OutVertex points[3];
};

void clear(GPUMemory &mem, ClearCommand cmd);
void draw(GPUMemory &mem, DrawCommand cmd);
void bindFramebuffer(GPUMemory &mem, BindFramebufferCommand cmd);
void bindProgram(GPUMemory &mem, BindProgramCommand cmd);
void bindVertexArray(GPUMemory &mem, BindVertexArrayCommand cmd);
uint32_t computeVertexID(GPUMemory &mem, VertexArray const &vao,
                         uint32_t shaderInvocation);
void vertexAssembly(GPUMemory &mem, VertexArray const &vao, InVertex &inVertex);
void runPrimitiveAssembly(GPUMemory &mem, Triangle &triangle,
                          VertexArray const &vao, uint32_t t, Program &prg);
void runPerspectiveDivision(Triangle &triangle);
void runViewportTransformation(Triangle &triangle, uint32_t width,
                               uint32_t height);
void rasterizeTriangle(GPUMemory &mem, Framebuffer &fbo, Triangle &triangle,
                       Program &prg);
glm::vec3 computeBarycentrics(Triangle &triangle, float x, float y);
bool isInsideTriangle(const glm::vec3 &barycentrics);
bool isBackFacing(Triangle &triangle);
void createFragment(InFragment &inFragment, Triangle &triangle,
                    glm::vec3 const &barycentrics, float x, float y,
                    Program &prg);
void perFragmentOperations(Framebuffer &fbo, OutFragment &outFragment,
                           InFragment &inFragment);

//! [izg_enqueue]
void izg_enqueue(GPUMemory &mem, CommandBuffer const &cb) {
  (void)mem;
  (void)cb;

  mem.gl_DrawID = 0;

  for (uint32_t i = 0; i < cb.nofCommands; ++i) {
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;

    if (type == CommandType::CLEAR) {
      clear(mem, data.clearCommand);
    } else if (type == CommandType::DRAW) {
      draw(mem, data.drawCommand);
    } else if (type == CommandType::SET_DRAW_ID) {
      mem.gl_DrawID = data.setDrawIdCommand.id;
    } else if (type == CommandType::BIND_FRAMEBUFFER) {
      bindFramebuffer(mem, data.bindFramebufferCommand);
    } else if (type == CommandType::BIND_PROGRAM) {
      bindProgram(mem, data.bindProgramCommand);
    } else if (type == CommandType::BIND_VERTEXARRAY) {
      bindVertexArray(mem, data.bindVertexArrayCommand);
    } else if (type == CommandType::SUB_COMMAND) {
      izg_enqueue(mem, *data.subCommand.commandBuffer);
    }
  }
}
//! [izg_enqueue]

void clear(GPUMemory &mem, ClearCommand cmd) {
  Framebuffer *fbo = mem.framebuffers + mem.activatedFramebuffer;

  if (cmd.clearColor) {
    if (fbo->color.data) {
      uint8_t clearColor[4] = {static_cast<uint8_t>(cmd.color.r * 255),
                               static_cast<uint8_t>(cmd.color.g * 255),
                               static_cast<uint8_t>(cmd.color.b * 255),
                               static_cast<uint8_t>(cmd.color.a * 255)};

      for (uint32_t y = 0; y < fbo->height; ++y) {
        for (uint32_t x = 0; x < fbo->width; ++x) {
          uint8_t *pixelStart = ((uint8_t *)fbo->color.data) +
                                y * fbo->color.pitch +
                                x * fbo->color.bytesPerPixel;

          uint8_t *pixelu = (uint8_t *)pixelStart;
          pixelu[0] = clearColor[0];
          pixelu[1] = clearColor[1];
          pixelu[2] = clearColor[2];
          pixelu[3] = clearColor[3];
        }
      }
    }
  }

  if (cmd.clearDepth) {
    if (fbo->depth.data) {
      for (uint32_t y = 0; y < fbo->height; ++y) {
        for (uint32_t x = 0; x < fbo->width; ++x) {
          float *depthPixel = ((float *)fbo->depth.data) +
                              y * fbo->depth.pitch / sizeof(float) + x;
          *depthPixel = cmd.depth;
        }
      }
    }
  }
}

void draw(GPUMemory &mem, DrawCommand cmd) {
  Program prg = mem.programs[mem.activatedProgram];
  VertexArray vao = mem.vertexArrays[mem.activatedVertexArray];
  Framebuffer fbo = mem.framebuffers[mem.activatedFramebuffer];

  for (uint32_t i = 0; i < cmd.nofVertices; i += 3) {
    Triangle triangle;
    runPrimitiveAssembly(mem, triangle, vao, i, prg);
    runPerspectiveDivision(triangle);
    runViewportTransformation(triangle, fbo.width, fbo.height);
    if (!(cmd.backfaceCulling && isBackFacing(triangle))) {
      rasterizeTriangle(mem, fbo, triangle, prg);
    }
  }

  mem.gl_DrawID++;
}

void runPrimitiveAssembly(GPUMemory &mem, Triangle &triangle,
                          VertexArray const &vao, uint32_t t, Program &prg) {
  for (uint32_t v = 0; v < 3; ++v) {
    InVertex inVertex;
    inVertex.gl_VertexID = computeVertexID(mem, vao, t + v);
    vertexAssembly(mem, vao, inVertex);
    ShaderInterface si;
    si.uniforms = mem.uniforms;
    si.textures = mem.textures;
    si.gl_DrawID = mem.gl_DrawID;
    prg.vertexShader(triangle.points[v], inVertex, si);
  }
}

void runPerspectiveDivision(Triangle &triangle) {
  for (uint32_t i = 0; i < 3; ++i) {
    triangle.points[i].gl_Position.x /= triangle.points[i].gl_Position.w;
    triangle.points[i].gl_Position.y /= triangle.points[i].gl_Position.w;
    triangle.points[i].gl_Position.z /= triangle.points[i].gl_Position.w;
  }
}

void runViewportTransformation(Triangle &triangle, uint32_t width,
                               uint32_t height) {
  for (uint32_t i = 0; i < 3; ++i) {
    float x = triangle.points[i].gl_Position.x;
    float y = triangle.points[i].gl_Position.y;

    triangle.points[i].gl_Position.x = (x * 0.5f + 0.5f) * width;
    triangle.points[i].gl_Position.y = (y * 0.5f + 0.5f) * height;
  }
}

void rasterizeTriangle(GPUMemory &mem, Framebuffer &fbo, Triangle &triangle,
                       Program &prg) {
  int minX = glm::max(
      0, static_cast<int>(glm::min(glm::min(triangle.points[0].gl_Position.x,
                                            triangle.points[1].gl_Position.x),
                                   triangle.points[2].gl_Position.x)));
  int minY = glm::max(
      0, static_cast<int>(glm::min(glm::min(triangle.points[0].gl_Position.y,
                                            triangle.points[1].gl_Position.y),
                                   triangle.points[2].gl_Position.y)));
  int maxX = glm::min(
      static_cast<int>(fbo.width) - 1,
      static_cast<int>(glm::max(glm::max(triangle.points[0].gl_Position.x,
                                         triangle.points[1].gl_Position.x),
                                triangle.points[2].gl_Position.x)));
  int maxY = glm::min(
      static_cast<int>(fbo.height) - 1,
      static_cast<int>(glm::max(glm::max(triangle.points[0].gl_Position.y,
                                         triangle.points[1].gl_Position.y),
                                triangle.points[2].gl_Position.y)));

  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      glm::vec3 barycentrics =
          computeBarycentrics(triangle, x + 0.5f, y + 0.5f);
      if (isInsideTriangle(barycentrics)) {
        InFragment inFragment;
        createFragment(inFragment, triangle, barycentrics, x + 0.5f, y + 0.5f,
                       prg);

        OutFragment outFragment;
        ShaderInterface si;
        si.uniforms = mem.uniforms;
        si.textures = mem.textures;
        si.gl_DrawID = mem.gl_DrawID;

        prg.fragmentShader(outFragment, inFragment, si);
        perFragmentOperations(fbo, outFragment, inFragment);
      }
    }
  }
}

glm::vec3 computeBarycentrics(Triangle &triangle, float x, float y) {
  float bcY =
      triangle.points[1].gl_Position.y - triangle.points[2].gl_Position.y;
  float acX =
      triangle.points[0].gl_Position.x - triangle.points[2].gl_Position.x;
  float cbX =
      triangle.points[2].gl_Position.x - triangle.points[1].gl_Position.x;
  float acY =
      triangle.points[0].gl_Position.y - triangle.points[2].gl_Position.y;
  float caY =
      triangle.points[2].gl_Position.y - triangle.points[0].gl_Position.y;

  float xcX = x - triangle.points[2].gl_Position.x;
  float ycY = y - triangle.points[2].gl_Position.y;

  float d = bcY * acX + cbX * acY;

  float u = (bcY * xcX + cbX * ycY) / d;
  float v = (caY * xcX + acX * ycY) / d;

  return glm::vec3(u, v, 1.0f - u - v);
}

bool isInsideTriangle(const glm::vec3 &barycentrics) {
  return barycentrics.x >= 0 && barycentrics.y >= 0 && barycentrics.z >= 0;
}

bool isBackFacing(Triangle &triangle) {
  float area =
      (triangle.points[1].gl_Position.x - triangle.points[0].gl_Position.x) *
          (triangle.points[2].gl_Position.y -
           triangle.points[1].gl_Position.y) -
      (triangle.points[2].gl_Position.x - triangle.points[1].gl_Position.x) *
          (triangle.points[1].gl_Position.y - triangle.points[0].gl_Position.y);

  return area < 0.0f;
}

void createFragment(InFragment &inFragment, Triangle &triangle,
                    glm::vec3 const &barycentrics, float x, float y,
                    Program &prg) {
  inFragment.gl_FragCoord.x = x;
  inFragment.gl_FragCoord.y = y;
  inFragment.gl_FragCoord.z =
      triangle.points[0].gl_Position.z * barycentrics.x +
      triangle.points[1].gl_Position.z * barycentrics.y +
      triangle.points[2].gl_Position.z * barycentrics.z;

  float h0 = triangle.points[0].gl_Position.w;
  float h1 = triangle.points[1].gl_Position.w;
  float h2 = triangle.points[2].gl_Position.w;

  float s = barycentrics.x / h0 + barycentrics.y / h1 + barycentrics.z / h2;

  float lambda0 = barycentrics.x / (h0 * s);
  float lambda1 = barycentrics.y / (h1 * s);
  float lambda2 = barycentrics.z / (h2 * s);

  for (uint32_t i = 0; i < maxAttributes; ++i) {
    AttributeType a = prg.vs2fs[i];
    if (a == AttributeType::EMPTY || a == AttributeType::UINT) {
      continue;
    } else if (a == AttributeType::FLOAT) {
      inFragment.attributes[i].v1 =
          triangle.points[0].attributes[i].v1 * lambda0 +
          triangle.points[1].attributes[i].v1 * lambda1 +
          triangle.points[2].attributes[i].v1 * lambda2;
    } else if (a == AttributeType::VEC2) {
      inFragment.attributes[i].v2 =
          triangle.points[0].attributes[i].v2 * lambda0 +
          triangle.points[1].attributes[i].v2 * lambda1 +
          triangle.points[2].attributes[i].v2 * lambda2;
    } else if (a == AttributeType::VEC3) {
      inFragment.attributes[i].v3 =
          triangle.points[0].attributes[i].v3 * lambda0 +
          triangle.points[1].attributes[i].v3 * lambda1 +
          triangle.points[2].attributes[i].v3 * lambda2;
    } else if (a == AttributeType::VEC4) {
      inFragment.attributes[i].v4 =
          triangle.points[0].attributes[i].v4 * lambda0 +
          triangle.points[1].attributes[i].v4 * lambda1 +
          triangle.points[2].attributes[i].v4 * lambda2;
    } else if (a == AttributeType::UVEC2) {
      inFragment.attributes[i].u2 =
          triangle.points[0].attributes[i].u2 * (uint)lambda0 +
          triangle.points[1].attributes[i].u2 * (uint)lambda1 +
          triangle.points[2].attributes[i].u2 * (uint)lambda2;
    } else if (a == AttributeType::UVEC3) {
      inFragment.attributes[i].u3 =
          triangle.points[0].attributes[i].u3 * (uint)lambda0 +
          triangle.points[1].attributes[i].u3 * (uint)lambda1 +
          triangle.points[2].attributes[i].u3 * (uint)lambda2;
    } else if (a == AttributeType::UVEC4) {
      inFragment.attributes[i].u4 =
          triangle.points[0].attributes[i].u4 * (uint)lambda0 +
          triangle.points[1].attributes[i].u4 * (uint)lambda1 +
          triangle.points[2].attributes[i].u4 * (uint)lambda2;
    }
  }
}

void perFragmentOperations(Framebuffer &fbo, OutFragment &outFragment,
                           InFragment &inFragment) {
  if (fbo.color.data == nullptr || fbo.depth.data == nullptr) {
    return;
  }

  float *depthPixel =
      ((float *)fbo.depth.data) +
      (uint32_t)inFragment.gl_FragCoord.y * fbo.depth.pitch / sizeof(float) +
      (uint32_t)inFragment.gl_FragCoord.x;

  uint8_t *pixel =
      ((uint8_t *)fbo.color.data) +
      (uint32_t)inFragment.gl_FragCoord.y * fbo.color.pitch +
      (uint32_t)inFragment.gl_FragCoord.x * fbo.color.bytesPerPixel;

  // Add small epsilon to prevent floating point errors
  float color[3] = {outFragment.gl_FragColor.r * 255.f + 0.00001f,
                    outFragment.gl_FragColor.g * 255.f + 0.00001f,
                    outFragment.gl_FragColor.b * 255.f + 0.00001f};

  float a = outFragment.gl_FragColor.a;

  float blendedColor[3] = {
      color[0] * a + pixel[0] * (1.f - a),
      color[1] * a + pixel[1] * (1.f - a),
      color[2] * a + pixel[2] * (1.f - a),
  };

  if (!outFragment.discard && inFragment.gl_FragCoord.z < *depthPixel) {
    pixel[0] = blendedColor[0];
    pixel[1] = blendedColor[1];
    pixel[2] = blendedColor[2];

    *depthPixel = inFragment.gl_FragCoord.z;
  }
}

void vertexAssembly(GPUMemory &mem, VertexArray const &vao,
                    InVertex &inVertex) {
  for (uint32_t j = 0; j < maxAttributes; ++j) {
    VertexAttrib a = vao.vertexAttrib[j];
    uint8_t *data = (uint8_t *)(mem.buffers[a.bufferID].data) + a.offset +
                    inVertex.gl_VertexID * a.stride;

    if (a.type == AttributeType::EMPTY) {
      continue;
    } else if (a.type == AttributeType::FLOAT) {
      inVertex.attributes[j].v1 = *reinterpret_cast<const float *>(data);
    } else if (a.type == AttributeType::VEC2) {
      inVertex.attributes[j].v2 = *reinterpret_cast<const glm::vec2 *>(data);
    } else if (a.type == AttributeType::VEC3) {
      inVertex.attributes[j].v3 = *reinterpret_cast<const glm::vec3 *>(data);
    } else if (a.type == AttributeType::VEC4) {
      inVertex.attributes[j].v4 = *reinterpret_cast<const glm::vec4 *>(data);
    } else if (a.type == AttributeType::UINT) {
      inVertex.attributes[j].u1 = *reinterpret_cast<const uint32_t *>(data);
    } else if (a.type == AttributeType::UVEC2) {
      inVertex.attributes[j].u2 = *reinterpret_cast<const glm::uvec2 *>(data);
    } else if (a.type == AttributeType::UVEC3) {
      inVertex.attributes[j].u3 = *reinterpret_cast<const glm::uvec3 *>(data);
    } else if (a.type == AttributeType::UVEC4) {
      inVertex.attributes[j].u4 = *reinterpret_cast<const glm::uvec4 *>(data);
    }
  }
}

uint32_t computeVertexID(GPUMemory &mem, VertexArray const &vao,
                         uint32_t shaderInvocation) {
  if (vao.indexBufferID < 0) {
    return shaderInvocation;
  }

  const char *bufferData =
      reinterpret_cast<const char *>(mem.buffers[vao.indexBufferID].data) +
      vao.indexOffset;

  if (vao.indexType == IndexType::UINT32) {
    const uint32_t *ind = reinterpret_cast<const uint32_t *>(bufferData);
    return ind[shaderInvocation];
  } else if (vao.indexType == IndexType::UINT16) {
    const uint16_t *ind = reinterpret_cast<const uint16_t *>(bufferData);
    return ind[shaderInvocation];
  } else if (vao.indexType == IndexType::UINT8) {
    const uint8_t *ind = reinterpret_cast<const uint8_t *>(bufferData);
    return ind[shaderInvocation];
  }

  // never reached
  return shaderInvocation;
}

void bindFramebuffer(GPUMemory &mem, BindFramebufferCommand cmd) {
  mem.activatedFramebuffer = cmd.id;
}

void bindProgram(GPUMemory &mem, BindProgramCommand cmd) {
  mem.activatedProgram = cmd.id;
}

void bindVertexArray(GPUMemory &mem, BindVertexArrayCommand cmd) {
  mem.activatedVertexArray = cmd.id;
}

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const &texture, glm::vec2 uv) {
  if (!texture.img.data)
    return glm::vec4(0.f);
  auto &img = texture.img;
  auto uv1 = glm::fract(glm::fract(uv) + 1.f);
  auto uv2 = uv1 * glm::vec2(texture.width - 1, texture.height - 1) + 0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture, pix);
}

/**
 * @brief This function reads color from texture with clamping on the borders.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_textureClamp(Texture const &texture, glm::vec2 uv) {
  if (!texture.img.data)
    return glm::vec4(0.f);
  auto &img = texture.img;
  auto uv1 = glm::clamp(uv, 0.f, 1.f);
  auto uv2 = uv1 * glm::vec2(texture.width - 1, texture.height - 1) + 0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture, pix);
}

/**
 * @brief This function fetches color from texture.
 *
 * @param texture texture
 * @param pix integer coorinates
 *
 * @return color 4 floats
 */
glm::vec4 texelFetch(Texture const &texture, glm::uvec2 pix) {
  auto &img = texture.img;
  glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f);
  if (pix.x >= texture.width || pix.y >= texture.height)
    return color;
  if (img.format == Image::UINT8) {
    auto colorPtr = (uint8_t *)getPixel(img, pix.x, pix.y);
    for (uint32_t c = 0; c < img.channels; ++c)
      color[c] = colorPtr[img.channelTypes[c]] / 255.f;
  }
  if (texture.img.format == Image::FLOAT32) {
    auto colorPtr = (float *)getPixel(img, pix.x, pix.y);
    for (uint32_t c = 0; c < img.channels; ++c)
      color[c] = colorPtr[img.channelTypes[c]];
  }
  return color;
}
