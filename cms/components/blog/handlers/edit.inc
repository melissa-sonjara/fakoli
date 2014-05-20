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

Fakoli::using("blog", "field_renderers", "taxonomy");
Fakoli::usingFeature("auto_form");

if (!Settings::checkPermission("settings", "editor_roles"))
{
	Fakoli::end("Access Denied");
}

$blog_id = checkNumeric($_GET["blog_id"]);

$blog = new Blog();

$menu_item = "Blogs";

if ($blog_id)
{
	$blog->load($blog_id);
	$title = "Editing Blog Definition for $blog->title";
}
else
{
	$title = "Add a new Blog";
	
	$_POST["owner_id"] = $user->get($user->getPrimaryKey());
	$_POST["created_date"] = now();
}

$blogView = new BlogView($blog);
$form = BlogManager::buildBlogForm($blog, $redirect, $blogView);
$form->ajaxSubmit("function(result) {new BlogManager().editResult(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->action = "/action/blog/edit?blog_id={$blog_id}";

TaxonomyManager::addTaxonomyFieldRenderers($form);

$form->allowDelete = $blog->allowDelete();
$form->onDelete = onDeleteCascade;

if ($method == "POST")
{
	if ($form->save())
	{
	    if ($form->deleted)
		{
			Fakoli::end("DELETED");
		}
		else
		{
			Fakoli::end("OK");
		}
	}
}

$script .= $form->writeScript();
$form->drawForm();

function onDeleteCascade($blog)
{
	$blog->delete();
	redirect("/admin/blogs");
}