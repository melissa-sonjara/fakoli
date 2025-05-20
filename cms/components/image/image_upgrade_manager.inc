<?php
/**\addtogroup image
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

/*
 * Manage component upgrades/updates to Image
 * component.
 * 
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *   
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 6/2/2011
 * 
 */

Fakoli::using("image", "component");

class ImageUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"sql:Initial Installation",
			"1.1"		=>	"sql:Added hidden flag to image_gallery",
			"1.2"		=>	"sql:Added related_resource field to image_record",
			"1.3"		=>	"sql:Added image_link_mode and thumbnail_size parameters to image_gallery",
			"1.4"		=>	"sql:Added fix_orientation flag to image_gallery",
			"1.5"		=>	"sql:Added thumbnail_format field to image_gallery",
			"1.6"		=>	"sql:Added randomize_filenames field to image_gallery",
			"1.7"		=>	"sql:Increase size of title and ALT_tag fields in image table",
			"1.8"		=>	"sql:Increase size of credits and caption fields in image table"
	);
	
	function __construct()
	{
		$this->component_name = "image";

		// sets the sqlFile name according to default
		parent::__construct();		
	}
}
/**@}*/?>
