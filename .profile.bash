# .profile.bash
# =============

export OSTYPE="darwin"
export SYSTEM="darwin"
export VISUAL="vim"
export EDITOR="$VISUAL"
export PATH="${PATH}:/opt/local/bin:/usr/bin/:${HOME}/bin:${HOME}/script"

. $RSTPATH/.profile/rst.bash
. $RSTPATH/.profile/base.bash
. $RSTPATH/.profile/general.bash
. $RSTPATH/.profile/superdarn.bash

. $RSTPATH/.profile/idl.bash

