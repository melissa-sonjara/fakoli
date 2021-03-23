<?php
/**\defgroup image_picker Image Picker
 * @{
 */


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

class ImagePickerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "image_picker";
		$component->description = "Image picker functionality for Fakoli/CMS RTE integration";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 5;
		$component->enabled = true;
		
		return $component;
	}
	
	static function subscribeToEvents()
	{
		return array("Initialize" => array(ImagePickerManifest, onInitialize),
					 "RegisterHTMLEditorPlugins" => array(ImagePickerManifest, registerPlugins));
	}	

	static function registerPlugins()
	{
		HtmlEditorManager::registerPlugin("Image Picker", "imagepicker");
	}
	
	static function onInitialize()
	{
		global $html_editor_extra_buttons;

		$html_editor_extra_buttons[] =
				array("name"	=>	"image_picker",
					  "image"	=>	"/fakoli/richtext/images/insert_picture.gif", 
					  "tooltip"	=>	"Insert Photos", 
					  "handler"	=>	"imagePicker");
	}
		
	static function getScripts()
	{
		return array("/components/image_picker/js/image_picker.js",
					 "/components/image_picker/js/image_picker_tinymce_plugin.js");
	}
	
	static function getStyles()
	{
		return array("/components/image_picker/css/image_picker.css");
	}
}

/**@}*/?>