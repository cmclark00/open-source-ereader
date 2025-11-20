import logging
import threading
import time

try:
    import RPi.GPIO as GPIO
    HAS_GPIO = True
except ImportError:
    HAS_GPIO = False

logger = logging.getLogger(__name__)

class InputManager:
    def __init__(self, callback):
        self.callback = callback
        self.running = False
        
        # Pin definitions (BCM) - matching the documentation we found
        self.KEYS = {
            'UP': 22,
            'DOWN': 5,
            'LEFT': 6,
            'RIGHT': 13,
            'SELECT': 26,
            'BACK': 27, # Using Page Back as Back
            'HOME': 19
        }
        
        if HAS_GPIO:
            GPIO.setmode(GPIO.BCM)
            for key, pin in self.KEYS.items():
                GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
                GPIO.add_event_detect(pin, GPIO.FALLING, callback=self._gpio_callback, bouncetime=200)
            logger.info("GPIO Input Manager initialized")
        else:
            logger.info("GPIO not available. Using keyboard simulation (stdin) if possible or mock.")

    def _gpio_callback(self, channel):
        for key, pin in self.KEYS.items():
            if pin == channel:
                self.callback(key)
                return

    def start(self):
        self.running = True
        if not HAS_GPIO:
            # Start a thread to read from stdin for testing on PC
            self.thread = threading.Thread(target=self._keyboard_loop)
            self.thread.daemon = True
            self.thread.start()

    def stop(self):
        self.running = False
        if HAS_GPIO:
            GPIO.cleanup()

    def _keyboard_loop(self):
        print("Keyboard Input Active. Controls: w(Up), s(Down), a(Left), d(Right), e(Select), q(Back)")
        while self.running:
            try:
                # This is a blocking call, so it might be hard to stop cleanly without a signal
                # For Windows testing, we might need a different approach or just rely on the user pressing keys followed by enter
                cmd = input().strip().lower()
                if cmd == 'w': self.callback('UP')
                elif cmd == 's': self.callback('DOWN')
                elif cmd == 'a': self.callback('LEFT')
                elif cmd == 'd': self.callback('RIGHT')
                elif cmd == 'e': self.callback('SELECT')
                elif cmd == 'q': self.callback('BACK')
            except EOFError:
                break
