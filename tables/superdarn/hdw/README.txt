Each radar has a distinct set of hardware parameters that are used
by the radar control software and the analysis software. These
parameters are read in a distinct order and are assumed to have
specific units. If either the order of the parameters or their units
are incorrect, the processing and analysis software will produce
incorrect answers that may not be easily identified. It is the
responsibility of the SuperDARN P.I.s to insure that the hdw.dat files
for their radars are correct and that these files are updated as
required to accurately represent the physical state of the radar and
that copies of these files are retained under revision control by the
Data Standards Working Group.

The hardware parameters are distributed as a string of values
delineated by one or more spaces. The following table specifies the
parameters, their units, and a brief description of their meaning.

01) Station ID (unique numerical value).
02) Status code (1 operational, -1 offline).
03) First date that parameter string is valid (YYYYMMDD).
04) First time that parameter string is valid (HH:MM:SS).
05) Geographic latitude of radar site (Given in decimal degrees to 5
    decimal places. Southern hemisphere values are negative)
06) Geographic longitude of radar site (Given in decimal degrees to
    5 decimal places. West longitude values are negative)
07) Altitude of the radar site (meters)
08) Physical scanning boresight (Direction of the center beam, measured in
    degrees relative to geographic north. CCW rotations are negative.)
09) Electronic shift to radar scanning boresight (Degrees relative to
    physical antenna boresight. Normally 0.0 degrees)
10) Beam separation (Angular separation in degrees between adjacent
    beams. Normally 3.24 degrees)
11) Velocity sign (At the radar level, backscattered signals with
    frequencies above the transmitted frequency are assigned positive
    Doppler velocities while backscattered signals with frequencies below
    the transmitted frequency are assigned negative Doppler velocity. This
    convention can be reversed by changes in receiver design or in the
    data sampling rate. This parameter is set to +1 or -1 to maintain the
    convention.)

In order to obtain information on the vertical angle of arrival of
the backscattered signals, the SuperDARN radars include a four antenna
interferometer array in addition to the 16 antenna main array. This
second array is typically displaced from the main array along the
array normal direction, and the different path length due to the
displacement and the different cable lengths between the antenna
arrays and their phasing matrices introduces a phase shift that is
dependent on the elevation angle of the returning backscattered
signal.

12) Phase sign (Cabling errors can lead to a 180 degree shift of the
    interferometry phase measurement. +1 indicates that the sign is
    correct, -1 indicates that it must be flipped.)
13) Tdiff [Channel A] (Propagation time from interferometer array antenna to
    phasing matrix input minus propagation time from main array antenna
    through transmitter to phasing matrix input. Units are decimal
    microseconds)
14) Tdiff [Channel B] (Propagation time from interferometer array antenna to
    phasing matrix input minus propagation time from main array antenna
    through transmitter to phasing matrix input. Units are decimal
    microseconds)
15) Interferometer X offset (Displacement of midpoint of
    interferometer array from midpoint of main array, along the line of
    antennas with +X toward higher antenna numbers. Units are meters)
16) Interferometer Y offset (Displacement of midpoint of
    interferometer array from midpoint of main array, along the array
    normal direction with +Y in the direction of the array normal. Units
    are meters)
17) Interferometer Z offset (Displacement of midpoint of
    interferometer array from midpoint of main array, in terms of altitude
    difference with +Z up. Units are meters)

Some SuperDARN radars have analog receivers whereas others have
analog front-end receivers followed by digital receivers. Analog
receivers and analog front-ends can have gain and bandwidth controls
that are identified here and corrected in the radar control software.
Digital receiver information is retained and compensated for within
the digital receiver driver.

18) Analog Rx rise time (Time given in microseconds. Time delays of
    less than ~10 microseconds can be ignored. If narrow-band filters are
    used in analog receivers or front-ends, the time delays should be
    specified.)
19) Analog Rx attenuator step (dB)
20) Analog attenuation stages (Number of stages. This is used for
    gain control of an analog receiver or front-end.)

21) Maximum number of range gates used (Number of gates. This is used for
    allocation of array storage.)
22) Maximum number of beams to be used at a particular radar site.
    (Number of beams. It is important to specify the true maximum. This
    will assure that a given beam number always points in the same
    direction. A subset of these beams, e.g. 8-23, can be used for
    standard 16 beam operation.)
