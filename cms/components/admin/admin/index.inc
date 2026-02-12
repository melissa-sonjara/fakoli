<?
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/
 
$title = "Site Administration";


$signature = ComponentManager::getComponentSignature();

?>
<b>Welcome <?echo $user->getFullName()?>!</b>
<br/>
<br/>
<strong>Server Time: </strong><?echo date("Y-m-d H:i:s T P")?><br/>
<strong>Server Timezone: </strong><?echo date_default_timezone_get()?><br/><br/>
<strong>Server Load: </strong><?echo Fakoli::getServerLoad()?><br/><br/>
<strong>Disk Usage:</strong><br/><br/><?echo Fakoli::getFileSystemInfo()?><br/><br/>
<!-- <strong># Currently Active Sessions: </strong><?echo Fakoli::countActiveSessions();?><br/><br/>
<strong>Component Configuration Signature:</strong> <?echo $signature?>-->
<br/>
<br/>
<a class="button" href="/action/component/scan"> Scan for Component Updates </a>&nbsp;&nbsp;
<a href='#' onclick='modalPopup("PHP Configuration", "/action/admin/php_info", "800px"); return false;' class="button">Show PHP Configuration</a>&nbsp;&nbsp;
<a href='#' onclick='modalPopup("PHP Configuration", "/action/admin/dump_cache", "800px"); return false;' class="button">Examine Cache</a>&nbsp;&nbsp;
<a class='button' href='/action/optimize/clear'>Clear CSS and Javascript Cache</a>&nbsp;&nbsp;
<a hef='#' onclick='modalPopup("Blocked IP Addresses", "/action/admin/blocked_ips", "800px"); return false;' class='button'>Show Blocked IPs</a>&nbsp;&nbsp;
<a href='/admin/user_form' class='button'>Create a New User</a>