# This script checks if SPINFO entry 4 is set.
# If yes, then "yes is print out", otherwise "no" is print out.
BEGIN {
   is_spinfo_block = 0
   is_ok = 1
}
{
   if ($1 ~ /^ *#/)
      next;

   if (tolower($0) ~ /block *spinfo/) {
      is_spinfo_block = 1
   } else if (tolower($0) ~ /block */) {
      is_spinfo_block = 0
   }

   if (is_spinfo_block) {
      if ($1 ~ / *4/)
         is_ok = 0
   }
}
END {
   if (is_ok == 1)
      print "yes";
   else
      print "no"
}
