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
 
class ImageManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "image";
		$component->description = "Image management for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 3;
		$component->enabled = true;
		
		return $component;
	}
		
	var $allow_sessionless_handlers = true;
	
	static function getAdminMenu()
	{
		return array
		(
			"Site Content"	=> array
			(

				"Images"	=>	array("page"		=>	"/admin/image_galleries",
								  "role"		=>	"admin,editor",
								  "weight"	=>	8,
								  "icon"		=>	"images")
			)
		);		
	}
	
	static function getScripts()
	{
		return array("/components/image/js/image_grid.js",
				"/components/image/js/image_manager.js"
				);
	}
	
	static function getStyles()
	{
		return array("/components/image/css/image.css");
	}
	
	static function subscribeToEvents()
	{
		return array(
			"ResolveIdentifier" 			=>	array(ImageManager, displayImageGallery),
			"EnumerateItems"	 			=>	array(ImageManager, enumerateItems),
			"ComponentScanComplete"			=>	array(ImageManager, setDefaults),
			"upgradeComponent"				=>	array(ImageManager, upgradeComponent),
			"RegisterSerializationHandler"	=>	array(ImageManager, registerSerializationHandler),
			"RegisterTaxonomyClasses"		=>	array(ImageManager, registerTaxonomyClasses),
			"DeleteUser"					=>	array(ImageManager, deleteUser),
		);
		
	}

}

?>