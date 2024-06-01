/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include "student/fwd.hpp"
#include <cstdint>
#include <student/gpu.hpp>
#include <student/prepareModel.hpp>

///\endcond

void prepareNode(GPUMemory &mem, CommandBuffer &cb, Node const &node,
                 Model const &model, glm::mat4 const &matrix,
                 uint32_t &drawCounter);

/**
 * @brief This function prepares model into memory and creates command buffer
 *
 * @param mem gpu memory
 * @param commandBuffer command buffer
 * @param model model structure
 */
//! [drawModel]
void prepareModel(GPUMemory &mem, CommandBuffer &commandBuffer,
                  Model const &model) {
  (void)mem;
  (void)commandBuffer;
  (void)model;

  for (uint32_t i = 0; i < model.buffers.size(); ++i) {
    Buffer buffer = model.buffers[i];
    mem.buffers[i] = buffer;
  }

  for (uint32_t i = 0; i < model.textures.size(); ++i) {
    Texture texture = model.textures[i];
    mem.textures[i] = texture;
  }

  uint32_t drawCounter = 0;
  glm::mat4 matrix = glm::mat4(1.f);
  for (size_t i = 0; i < model.roots.size(); ++i) {
    prepareNode(mem, commandBuffer, model.roots[i], model, matrix, drawCounter);
  };
}
//! [drawModel]

void prepareNode(GPUMemory &mem, CommandBuffer &cb, Node const &node,
                 Model const &model, glm::mat4 const &matrix,
                 uint32_t &drawCounter) {
  if (node.mesh >= 0) {
    Mesh mesh = model.meshes[node.mesh];

    VertexArray vao;
    vao.indexBufferID = mesh.indexBufferID;
    vao.indexOffset = mesh.indexOffset;
    vao.indexType = mesh.indexType;
    vao.vertexAttrib[0] = mesh.position;
    vao.vertexAttrib[1] = mesh.normal;
    vao.vertexAttrib[2] = mesh.texCoord;

    mem.vertexArrays[drawCounter] = vao;

    pushBindVertexArrayCommand(cb, drawCounter);
    pushDrawCommand(cb, mesh.nofIndices, !mesh.doubleSided);

    glm::mat4 newMatrix = matrix * node.modelMatrix;
    glm::mat4 inversedTransposedMatrix =
        glm::transpose(glm::inverse(newMatrix));

    mem.uniforms[10 + drawCounter * 5 + 0].m4 = newMatrix;
    mem.uniforms[10 + drawCounter * 5 + 1].m4 = inversedTransposedMatrix;
    mem.uniforms[10 + drawCounter * 5 + 2].v4 = mesh.diffuseColor;
    mem.uniforms[10 + drawCounter * 5 + 3].i1 = mesh.diffuseTexture;
    mem.uniforms[10 + drawCounter * 5 + 4].v1 = mesh.doubleSided;

    drawCounter++;
  }

  for (size_t i = 0; i < node.children.size(); ++i) {
    prepareNode(mem, cb, node.children[i], model, matrix * node.modelMatrix,
                drawCounter);
  }
}

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param si shader interface
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex &outVertex, InVertex const &inVertex,
                            ShaderInterface const &si) {
  (void)outVertex;
  (void)inVertex;
  (void)si;
  /// \todo Tato funkce reprezentujte vertex shader.<br>
  /// Vaším úkolem je správně trasnformovat vrcholy modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param si shader interface
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment &outFragment,
                              InFragment const &inFragment,
                              ShaderInterface const &si) {
  (void)outFragment;
  (void)inFragment;
  (void)si;
  /// \todo Tato funkce reprezentujte fragment shader.<br>
  /// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí
  /// lambertova osvětlovacího modelu. Bližší informace jsou uvedeny na hlavní
  /// stránce dokumentace.
}
//! [drawModel_fs]
