<?php

$rtn = checkNumeric($_GET["rtn"]);


if($rtn == 1)
	echo "<p><b>Reminders emails have been sent.</b></p>\n";
elseif($rtn == 2)
	echo "<div id='warning'>Reminders emails could not be sent.</div>\n";



