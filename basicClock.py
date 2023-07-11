#
# basicClock.py
#

import os
import sst

clockComponent = sst.Component("ClockComponent", "basicComponent.basicClock")
clockComponent.addParams({
  "clockFreq"  : "1.5Ghz",
  "clockTicks" : 1000
})
