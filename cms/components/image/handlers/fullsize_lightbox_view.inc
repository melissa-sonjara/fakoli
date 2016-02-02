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
 
Fakoli::using("image");

$image_id = checkNumeric($_GET["image_id"]);


if (!$image_id)
{
	$img = $_GET["img"];
	if ($img)
	{
		$image = ImageRecord::findByFilename($img);
		if ($image) 
		{
			$image_id = $image->image_id;
		}
		else
		{
			throw new FakoliException("Unknown Image");
		}
	}
}
else
{
	$image = new ImageRecord($image_id);
}

$gallery = $image->Gallery();
if ($gallery->image_link_mode == "Lightbox" && $image->related_resource)
{
	$link = "<a href='{$image->related_resource}'>";
	$closeLink = "</a>";
}
?>
<div style="width:100%; height:100%; vertical-align: middle; background: url(/fakoli/images/loader.gif) center center no-repeat #fff">
<?echo $link?><img src="/action/image/show?image_id=<?echo $image_id?>&size=600" 
				   style="max-height:100%;max-width:100%;position:absolute;top:50%;left:50%;transform:translate(-50%,-50%)" 
				   alt="<?echo $image->ALT_tag?>"/><?echo $closeLink?>
</div>