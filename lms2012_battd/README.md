Battery monitoring daemon
=========================

Rationale
---------
This subprogram is used for battery monitoring. Because it contains
code copied directly from LMS2012 (NiMH temperature model etc.),
it cannot be linked into the main NXT VM due to license incompatibility.
This is because according to
[Fedora](https://fedoraproject.org/wiki/Licensing/LOSLA), LOSLA is not
compatible with the GPL.

Therefore, this program forms a standalone module copyrighted under GPLv2.0.
It should be possible to reuse it outside of NXT3; for example, it could
be used for monitoring the battery for C4EV3 apps.

What does it do
---------------

What this daemon does is that it periodically reads voltage & current
measured by the ADC chip and it runs safety checks on these values. If some
limits are exceeded, first a warning is produced and if the problem
worsens, it forcefully shuts down the brick (just like LMS2012).

Currently the following checks are present:

 * "Over-discharge" protection (both battery pack and AA batteries).
   The thresholds come from the stock LMS2012 firmware source code.
 * Battery overheating protection (AA batteries only). This relies on
   the battery model from the stock firmware. If the batteries are thought
   to be 30 °C hotter than their surroundings, the brick will be shut down.
