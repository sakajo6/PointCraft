#include "polyscope/polyscope.h"

#include "polyscope/messages.h"
#include "polyscope/point_cloud.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/view.h"

#include <iostream>
#include <random>
#include <string>

#include "args/args.hxx"
#include "json/json.hpp"

#include "surface.hpp"
#include "gui.hpp"
#include "point_cloud.hpp"
#include "constants.hpp"

GuiManager guiManager;

void callback() {
  guiManager.enableAdminToolWindow();   // Admin Tool Window
  guiManager.enableEditingToolWindow(); // Editing Tool Window
  guiManager.enableLogWindow();         // Log Window
}

int main(int argc, char **argv) {
  // Configure the argument parser
  args::ArgumentParser parser("Point cloud editor tool"
                              "");

  // Flag arguments
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Flag downsample(parser, "downsample", "execute downsampling during initialization", {"downsample"});  // --downsample
  args::Flag debugMode(parser, "debug_mode", "enable debug mode (display the left side panel)", {"debug"});   // --debug
  args::Flag sketchMode(parser, "sketch_mode", "enable sketch interpolation tool", {"sketch"});               // --sketch
  args::Flag sprayMode(parser, "spray_mode", "enable spray interpolation tool", {"spray"});                   // --spray

  // Positional arguments
  args::Positional<std::string> inFile(parser, "mesh", "input mesh position");
  args::Positional<int> userID(parser, "userID", "userID position");

  // Parse args
  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  } catch (args::ParseError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  } catch (args::ValidationError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  // Options
  polyscope::view::windowWidth = WindowWidth;
  polyscope::view::windowHeight = WindowHeight;
  polyscope::view::bgColor = BackgroundColor;

  polyscope::state::lengthScale = 1.0;

  polyscope::options::autocenterStructures = false;
  polyscope::options::autoscaleStructures = false;
  polyscope::options::automaticallyComputeSceneExtents = false;
  polyscope::options::groundPlaneMode = polyscope::GroundPlaneMode::None;
  
  // Debug Mode
  if (debugMode) {
    polyscope::options::buildGui = true;
    polyscope::options::buildDefaultGuiPanels = true;
  }
  else {
    polyscope::options::buildGui = false;
    polyscope::options::buildDefaultGuiPanels = false;
  }

  // Initialize polyscope
  polyscope::init();

  // Initialize classes
  int currentMode;
  int currentSurfaceMode;

  // Poin Cloud Class
  PointCloud pointCloud(args::get(inFile), downsample);

  // Sketch Interpolation Tool Class
  SketchInterpolationTool sketchInterpolationTool(&currentMode, &pointCloud);
  SketchInterpolationTool* sketchInterpolationToolPtr = nullptr;
  if (sketchMode) sketchInterpolationToolPtr = &sketchInterpolationTool;

  // Spray Interpolation Tool Class
  SprayInterpolationTool  sprayInterpolationtool(&currentMode, &pointCloud);
  SprayInterpolationTool* sprayInterpolationToolPtr = nullptr;
  if (sprayMode) sprayInterpolationToolPtr = &sprayInterpolationtool;

  // Delete Tool Class
  DeleteTool deleteTool(&currentMode, &pointCloud);

  guiManager = GuiManager(
    args::get(inFile),
    args::get(userID),
    debugMode,

    &currentMode, 
    &currentSurfaceMode, 

    &pointCloud, 
    
    sketchInterpolationToolPtr,
    sprayInterpolationToolPtr,
    &deleteTool
  );

  // Render point cloud surface (pseudo surface and greedy surface)
  Surface pointCloudSurface(pointCloud.getVertices(), pointCloud.getNormals());
  pointCloudSurface.renderPointCloudSurface(
    GreedyProjName,
    PseudoSurfaceName,
    pointCloud.getAverageDistance(),
    false
  );

  // Add the callback
  polyscope::state::userCallback = callback;

  // Show the gui
  polyscope::show();

  return 0;
}
