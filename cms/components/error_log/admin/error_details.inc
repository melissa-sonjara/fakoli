<?php
Fakoli::using("error_log", "user");
Fakoli::usingFeature("auto_form");

$error_log_id = checkNumeric($_GET["error_log_id"]);

$error = new ErrorLog($error_log_id);

?>
<b>Error encountered by <?echo $error->getUserFullName()?> on <?echo $error->timestamp?>.</b>
<p><b>Session ID: <?echo $error->session?></b></p>
<p><b>URI:</b> <?echo $error->uri?>, <b>Referer:</b> <?echo $error->referer?></p> 
<p><?echo $error->message?></p>
<p><em><?echo $error->file?></em> Line <?echo $error->line?></p>
<h4>Stack Trace</h4>
<p><?echo formatAsHTML($error->details)?></p>
<br/>
<br/>
<a class='button' href='error_log'>&laquo; Back to Error Log</a>
