<?php
Fakoli::using("developer_tools");

$title = "Test PHP CLI Settings";
$menu_item = "Test PHP CLI";

?>
<p>Use this page to test the command-line configuration for PHP to ensure that any background processes launched via Fakoli's <code>process</code> component will run correctly.
Clicking on the button below should show a progress dialog that runs from 0% to 100%. If the progress bar does not appear, your PHP CLI configuration and/or the Application Settings
for the <code>process</code> component will need to be fixed.</p>
<br/>
<a class='button' href='#' onclick='new BackgroundProcess("Running Test Process", "/action/developer_tools/test_background_process"); return false;'>Test Background Processing</a>