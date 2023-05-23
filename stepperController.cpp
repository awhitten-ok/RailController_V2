#include <PubSubClient.h>
#include <AccelStepper.h>
#include "stepperController.h"

int RAIL_NUMBER = 2;
#define stepPin 14
#define dirPin 12 // Define a stepper and the pins it will use
#define motorInterfaceType 1
#define GET_VARIABLE_NAME(Variable) (#Variable)
stepperController::stepperController()
{
  stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
  SwitchState = 0;
  ProxState = 0;
  hasRun = 0;
}

// Getters and Setters
AccelStepper stepperController::getStepper()
{
  return stepper;
}
int stepperController::getProxState()
{
  return ProxState;
}
int stepperController::getSwitchState()
{
  return SwitchState;
}
int stepperController::getHasRun()
{
  return hasRun;
}
int stepperController::getCurrentPosition()
{
  return stepper.currentPosition();
}
void stepperController::setHasRun(int state)
{
  hasRun = state;
}
void stepperController::setProxState(int newProxState)
{
  ProxState = newProxState;
}

// Setups Stepper Motor
void stepperController::stepperSetup()
{
  Serial.println("stepper startup");
  stepper.setMaxSpeed(2000);      // set max speed the motor will turn (steps/second)
  stepper.setAcceleration(20000); // set acceleration (steps/second^2)
  Serial.println(stepper.maxSpeed());
  pinMode(Enable, OUTPUT); // Configure pin 13 (enable pin) as output
  digitalWrite(Enable, HIGH);
  pinMode(Prox, INPUT); // Configure pin D2 (Prox pin) as input
  pinMode(Lock, INPUT); // Configure pin D1 (Lock Signal) as output
  yield();
}

// Moves sled to desired position, publishes result back to User Client
String stepperController::moveToPosition(long positionValue)
{
  Serial.println("In moveToPosition");
  digitalWrite(dirPin, LOW);
  digitalWrite(Enable, LOW);
  std::string message;

  Serial.println(stepper.maxSpeed());
  stepper.moveTo(positionValue);

  delay(0);

  yield();
  Serial.println("Going to :");
  Serial.print(positionValue);
  while (stepper.distanceToGo() != 0)
  { // Moves phone to lock
    SwitchState = digitalRead(Switch);
    ProxState = digitalRead(Prox);

    // If switch on either side of rail is triggered, run calibration.
    if (ProxState == HIGH || SwitchState == HIGH)
    {
      Serial.println("ProxState: ");
      Serial.println(ProxState);
      Serial.println("SwitchState: ");
      Serial.println(SwitchState);
      calibration();
      yield();
      delay(0);
    }

    stepper.run();
    Serial.println("Moving to lock");
    yield();
    delay(0);
  }

  // Checks if the move was successful.
  if (stepper.currentPosition() == positionValue)
  {
    message = "99";
    yield();
    delay(0);
  }
  else
  {
    message = "-2";
    yield();
    delay(0);
  }
  Serial.println(stepper.currentPosition());
  delay(0);
  yield();
  return message;
}

// Calibration runs once upon reset of ESP
 stepperController::calibration()
{
  digitalWrite(Enable, LOW);
  Serial.println("Spinning Counter-Clockwise...");
  ProxState = digitalRead(Prox);
  Serial.println("ProxState");
  Serial.println(ProxState);
  digitalWrite(dirPin, LOW);
  while (ProxState != HIGH) // view recent arduino forum post section 28 for other motor step options turnMot
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    ProxState = digitalRead(Prox);
    yield(); // keeps the watchdog timer from crashing
  }
  stepper.setCurrentPosition(-50);
  stepper.moveTo(50);
  while (stepper.distanceToGo() != 0) // Moves phone to lock
  {
    stepper.run();
    delay(0);
  }
  Serial.println("Reset Zero Position");
  digitalWrite(Enable, HIGH);
  delay(1000);
  yield();
  hasRun = 1;
}
