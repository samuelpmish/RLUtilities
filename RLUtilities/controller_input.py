from inputs import get_gamepad  # pip install inputs
from rlbot.agents.base_agent import SimpleControllerState
import threading

def deadzone(normalized_axis):
    if abs(normalized_axis) < 0.1: return 0.0
    return normalized_axis

class ControllerInput:
    def __init__(self):
        self.aerial_control = 0
        self._gas_pedal = 0.0
        self._brake_pedal = 0.0

        self._stick_x = 0.0
        self._stick_y = 0.0

        self.jump = 0
        self.boost = 0
        self.handbrake = 0
        self.L1 = 0

    @property
    def throttle(self):
        return self._gas_pedal - self._brake_pedal

    @property
    def steer(self):
        return self._stick_x

    @property
    def roll(self):
        return self._stick_x if self.aerial_control else 0.0

    @property
    def yaw(self):
        return 0.0 if self.aerial_control else self._stick_x

    @property
    def pitch(self):
        return self._stick_y

    def get_output(self):
        output = SimpleControllerState()

        output.throttle = self.throttle
        output.steer = self.steer
        output.pitch = self.pitch
        output.yaw = self.yaw
        output.roll = self.roll
        output.jump = self.jump
        output.boost = self.boost
        output.handbrake = self.handbrake

        return output

    def main_poll_loop(self):
        while 1:
            events = get_gamepad()  # Blocking
            for event in events:
                #print(repr((event.ev_type, event.code, event.state)))
                if False: pass
                elif event.ev_type=='Absolute' and event.code=='ABS_RZ': self._gas_pedal = deadzone(event.state / 255.0)
                elif event.ev_type=='Absolute' and event.code=='ABS_Z': self._brake_pedal = deadzone(event.state / 255.0)
                elif event.ev_type=='Absolute' and event.code=='ABS_X': self._stick_x = deadzone(event.state / 32768.0)
                elif event.ev_type=='Absolute' and event.code=='ABS_Y': self._stick_y = deadzone(-event.state / 32768.0)
                elif event.ev_type=='Key' and event.code=='BTN_SOUTH': self.jump = event.state
                elif event.ev_type=='Key' and event.code=='BTN_TL': self.L1 = event.state
                elif event.ev_type=='Key' and event.code=='BTN_TR': self.aerial_control = event.state
                elif event.ev_type=='Key' and event.code=='BTN_EAST': self.boost = event.state
                elif event.ev_type=='Key' and event.code=='BTN_WEST': self.handbrake = event.state

controller = ControllerInput()
threading.Thread(target=controller.main_poll_loop, daemon=True).start()
