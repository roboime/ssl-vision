//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    plugin_detect_robots.cpp
  \brief   C++ Implementation: plugin_detect_robots
  \author  Author Name, 2009
*/
//========================================================================
#include "plugin_detect_robots.h"

PluginDetectRobots::PluginDetectRobots(FrameBuffer * _buffer, LUT3D * lut, const CameraParameters& camera_params, const RoboCupField& field, CMPattern::TeamSelector * _global_team_selector_blue, CMPattern::TeamSelector * _global_team_selector_yellow)
 : VisionPlugin(_buffer), camera_parameters(camera_params), field(field)
{
  _lut=lut;

  color_id_yellow = _lut->getChannelID("Yellow");
  if (color_id_yellow == -1) printf("WARNING color label 'Yellow' not defined in LUT!!!\n");
  color_id_blue = _lut->getChannelID("Blue");
  if (color_id_yellow == -1) printf("WARNING color label 'Blue' not defined in LUT!!!\n");  

  global_team_selector_blue=_global_team_selector_blue;
  global_team_selector_yellow=_global_team_selector_yellow;
  
  team_detector_blue=new CMPattern::TeamDetector(_lut,camera_params,field);
  team_detector_yellow=new CMPattern::TeamDetector(_lut,camera_params,field);
  
  _settings=new VarList("Robot Detection");
}

PluginDetectRobots::~PluginDetectRobots()
{
}


VarList * PluginDetectRobots::getSettings() {
  return _settings;
}

string PluginDetectRobots::getName() {
  return "DetectRobots";
}

ProcessResult PluginDetectRobots::process(FrameData * data, RenderOptions * options)
{
  (void)options;
  if (data==0) return ProcessingFailed;

  SSL_DetectionFrame * detection_frame = 0;

  detection_frame=(SSL_DetectionFrame *)data->map.get("ssl_detection_frame");
  if (detection_frame == 0) detection_frame=new SSL_DetectionFrame();

  //acquire orange region list from data-map:
  CMVision::ColorRegionList * colorlist;
  colorlist=(CMVision::ColorRegionList *)data->map.get("cmv_colorlist");
  if (colorlist==0) {
    printf("error in robot detection plugin: no region-lists were found!\n");
    return ProcessingFailed;
  }

  //acquire color-labeled image from data-map:
  const Image<raw8> * image = (Image<raw8> *)(data->map.get("cmv_threshold"));
  if (image==0) {
    printf("error in robot detection plugin: no color-thresholded image was found!\n");
    return ProcessingFailed;
  }

  CMPattern::Team * team=0;
  ::google::protobuf::RepeatedPtrField< ::SSL_DetectionRobot >* robotlist=0;
  
  int color_id;
  int num_robots;
  CMPattern::TeamDetector * detector;
  //TODO: lookup color label from LUT

  for (int team_i = 0; team_i < 2; team_i++) {
    //team_i: 0==blue, 1==yellow
    if (team_i==0) {
      color_id=color_id_blue;
      team=global_team_selector_blue->getSelectedTeam();
      num_robots=global_team_selector_blue->getNumberRobots();
      detection_frame->clear_robots_blue();
      robotlist=detection_frame->mutable_robots_blue();
      detector=team_detector_blue;
    } else {
      color_id=color_id_yellow;
      team=global_team_selector_yellow->getSelectedTeam();
      num_robots=global_team_selector_yellow->getNumberRobots();
      detection_frame->clear_robots_yellow();
      robotlist=detection_frame->mutable_robots_yellow();
      detector=team_detector_yellow;
    }
    detector->init(team);

    detector->update(robotlist, color_id,  num_robots, image, colorlist);

    printf("DETECTED %d robots on team %d\n",robotlist->size(),team_i);

    fflush(stdout);
  }
  return ProcessingOk;

}

