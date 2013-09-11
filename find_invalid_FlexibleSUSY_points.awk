BEGIN {
   ss_error = 7;
   fs_error = 9;
   sp_error = 11;
   sm_error = 13;
}
{
   if ($1 ~ /^ *#/)
      next;

   if ($7 == 0 && $9 != 0 && $11 == 0 && $13 == 0) {
      print "Block MINPAR # data line " NR
      print "   1   " $1
      print "   2   " $2
      print "   3   " $3
      print "   4   " $4
      print "   5   " $5
      print ""
   }
}
