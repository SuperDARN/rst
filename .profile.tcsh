# .profile.tcsh
# =============

setenv OSTYPE "linux"
setenv SYSTEM "linux"
setenv VISUAL "vim"
setenv EDITOR "$VISUAL"
setenv PATH "${PATH}:/opt/local/bin:/usr/bin/:${HOME}/bin:${HOME}/script"

source $RSTPATH/.profile/rst.tcsh
source $RSTPATH/.profile/base.tcsh
source $RSTPATH/.profile/general.tcsh
source $RSTPATH/.profile/superdarn.tcsh

source $RSTPATH/.profile/idl.tcsh

