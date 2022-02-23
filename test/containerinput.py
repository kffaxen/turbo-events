import datetime
import time
import TurboEvents
t = TurboEvents.TurboEvents(False)
t.addPrintOutput()
t.addEvent(datetime.datetime.fromtimestamp(time.time()), 'Hello,')
time.sleep(0.3)
t.addEvent(datetime.datetime.fromtimestamp(time.time()), 'World!')
t.createContainerInput()
t.run(1.000000)
