<?php
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

Fakoli::using("auto_login");
Fakoli::usingFeature("data_view");

$tokens = Query::create('AuthenticationToken', "")->execute();

$menu_item = "Authentication Tokens";

function formatEnabled($token)
{
	$img = $token->enabled ? "on.png" : "off.png";
	$alt = $token->enabled ? "Enabled" : "Disabled";

	return "<img src='/fakoli/images/$img' id='enable_{$token->authentication_token_id}' alt='$alt' style='display: inline-block; vertical-align: middle; border: none;'/>";
}
	
$table = new DataListView($tokens, "tokens");
$table->column("User", "<a href='/admin/authentication_token_form?authentication_token_id={authentication_token_id}'>{User.username} ({User.first_name} {User.last_name})</a>", true, "width: 25%")
	  ->column("Role", "{User.role}", "width: 15%")
	  ->column("Token", "{token}", true, "width: 40%")
	  ->column("Last Access", "{last_access:m/d/Y g:ia}", true, "width: 15%")
	  ->column("Enabled", formatEnabled, false, "text-align: center");
	  
$table->emptyMessage = "No authentication tokens have been generated.";
$table->pageSize = 10;
$table->filter = true;
$table->excelFile = "authentication_tokens.xls";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href="/admin/authentication_token_form" class="button">Generate a New Token</a>
