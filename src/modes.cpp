#include "polyscope/polyscope.h"

#include "polyscope/point_cloud.h"
#include "polyscope/surface_mesh.h"

#include "modes.hpp"
#include "surface.hpp"
#include "constants.hpp"

void ModeSelector::enableModeSelection(ImGuiIO &io) {
  // Export .obj file
  ImGui::Text("Export file:");
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button(".obj")) pointCloud->exportOBJFile();

  // Undo/Redo
  ImGui::Text("\nUndo/Redo:");
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button("<< Undo")) pointCloud->executeUndo();
  ImGui::SameLine();
  if (ImGui::Button("Redo >>")) pointCloud->executeRedo();

  // Tool Selection
  ImGui::Text("\nTool Selection:");
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button("Reset Tool")) {
    *currentMode = MODE_NONE;
  }
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button("Sketch Interpolation Tool")) {
    *currentMode = MODE_SKETCH_INTERPOLATION;
    sketchInterpolationTool->initSketch();
  }
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button("Spray Interpolation Tool")) {
    *currentMode = MODE_SPRAY_INTERPOLATION;
    sprayInterpolationTool->initSketch();
  }
  ImGui::Text("   "); ImGui::SameLine();
  if (ImGui::Button("Delete Tool")) {
    *currentMode = MODE_DELETION;
    deleteTool->initSketch();
  }

  switch (*currentMode) {
    case MODE_NONE:
      ImGui::Text("   Selected Tool: None");
      polyscope::view::moveScale = 1.0;
      break;
    case MODE_SKETCH_INTERPOLATION:
      ImGui::Text("   Selected Tool: Sketch Interpolation Tool");
      sketchInterpolationTool->launchToolOperation();
      break;
    case MODE_SPRAY_INTERPOLATION:
      ImGui::Text("   Selected Tool: Spray Interpolation Tool");
      sprayInterpolationTool->launchToolOperation();
      break;
    case MODE_DELETION:
      ImGui::Text("   Selected Tool: Delete Tool");
      deleteTool->launchToolOperation();
      break;
  }

  // Surface Visualization
  ImGui::Text("\nSurface Visualization:");
  ImGui::Text("   "); ImGui::SameLine();
  ImGui::RadioButton("None", currentSurfaceMode, SURFACE_MODE_NONE);
  ImGui::Text("   "); ImGui::SameLine();
  ImGui::RadioButton("Pseudo Surface", currentSurfaceMode, SURFACE_MODE_PSEUDO);
  ImGui::Text("   "); ImGui::SameLine();
  ImGui::RadioButton("Greedy Surface", currentSurfaceMode, SURFACE_MODE_GREEDY);

  polyscope::SurfaceMesh *pseudoSurface = polyscope::getSurfaceMesh(PseudoSurfaceName);
  polyscope::SurfaceMesh *greedySurface = polyscope::getSurfaceMesh(GreedyProjName);
  switch (*currentSurfaceMode) {
    case SURFACE_MODE_NONE:
      pseudoSurface->setEnabled(false);
      greedySurface->setEnabled(false);
      break;
    case SURFACE_MODE_PSEUDO:
      pseudoSurface->setEnabled(true);
      greedySurface->setEnabled(false);
      break;
    case SURFACE_MODE_GREEDY:
      pseudoSurface->setEnabled(false);
      greedySurface->setEnabled(true);
      break;
  }
}