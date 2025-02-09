/*!
 * @file
 * @brief This file contains function declarations for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>

void prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model);

void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si);

void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si);
