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

class AdminPageProxyView
{
	var $adminPage = null;
	
	function __construct()
	{
		global $isAction;
		global $isResource;
		global $isAdmin;
		
		global $identifier;
		global $page;
		global $section;
		global $config;
		
		// Security rules for AdminPageProxies
		
		// #0 - AdminPageProxyView can only be instantiated in a ComponentPage script - not inside an action handler, resource or admin page
		// #1 - admin page may only be exposed by a component page that has the same identifier (i.e. script has same name)
		// #2 - admin page must be in the same component as the component page creating the proxy
		// #3 - page must be served under SSL
		// #4 - user must have one of the privileged access roles (i.e. role must be able to access the admin section)

		try
		{
			$this->adminPage = AdminPage::fromIdentifier($identifier);
		}
		catch(DataNotFoundException $e)
		{
			throw new FakoliException("No matching admin page");
		}
		
		$adminComponent = $this->adminPage->Component();
		$hostComponent = $page->Component();
		
		$https = Fakoli::checkSSL();
		
		// #0
		if ($isAction || $isResource || $isAdmin)
		{
			throw new FakoliException("Incorrect execution context for proxy view");
		}
		
		// #1 
		if ($page->identifier != $this->adminPage->identifier)
		{
			throw new FakoliException("Proxy identifier mismatch");
		}
		
		// #2
		if ($adminComponent->component_id != $hostComponent->component_id)
		{
			throw new FakoliException("Proxy component mismatch");
		}
		
		// #3
		
		if (!$https)
		{
			throw new FakoliException("Connection not secured by SSL");	
		}
		
		// #4
		
		$adminAccess = $config["admin_access_roles"];
		if (!$adminAccess) $adminAccess = "admin";
		
		if (!checkRole($adminAccess))
		{
			throw new FakoliException("Insufficiently privileged");
		}
		
		$shim = AdminManager::getAdminProxyShim();
		if (is_callable($shim)) $shim();
	}
	
	function drawView()
	{
		global $config;
		global $method;
		global $user;
		global $script;
		global $styles;
		global $dialogs;
		global $page;
		global $menu_item;
		global $isAdmin;
		
		require_once $this->adminPage->server_path;
	}
}