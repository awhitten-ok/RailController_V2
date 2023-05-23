#ifndef stepperController_h
#define stepperController_h
#include <PubSubClient.h>
#include <AccelStepper.h>

class stepperController
{
private:
  AccelStepper stepper;
  const int StepsAcrossRail = 500;
  const int Enable = 13; // Create a variable holding enable pin address
  const int Prox = 4;    // Create variable holding proximity sensor pin address
  const int Lock = 5;    // Create variable holding microstepping pin address
  const int Switch = 15;
  int SwitchState;
  int ProxState;
  int hasRun;

public:
  stepperController();
  void stepperSetup();
  String moveToPosition(long positionValue);
  void calibration();
  int getSwitchState();
  int getProxState();
  int getHasRun();
  int getCurrentPosition();
  AccelStepper getStepper();

  void setProxState(int state);
  void setHasRun(int state);
};
#endif
