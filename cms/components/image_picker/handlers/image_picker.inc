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

$mode    = $_GET["Mode"];     // "insert" or "select" - "select" is default
$editor  = $_GET["Editor"];   // in "insert" mode - ID of the RTE to insert the image into
$field   = $_GET["Field"];    // in "select" mode - ID of the field to set the value of
$preview = $_GET["Preview"];  // in "select" mode - ID of the element to display the preview thumbnail.
$gallery_id = checkNumeric($_GET["gallery_id"]);

$title = ($mode == "insert") ? "Insert Images" : "Select an Image";

$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();

if (!$gallery_id && count($galleries) > 0)
{
	$gallery_id = $galleries[0]->gallery_id;
}

$styles .= <<<ENDSTYLES
<style type="text/css">
body
{
	text-align: left;
}

#preview
{
border:1px inset #888;
display:table-cell;
height:200px;
text-align:center;
vertical-align:middle;
width:200px;
margin-left: 20px;
-moz-border-radius: 8px;
border-radius: 8px;
background-color: #eee;
}
#searchblock
{
	width: 95%;
	height: 250px;
	border: solid 1px #000;
	overflow: auto;
	clear: left;
}

</style>
ENDSTYLES;

$script .= <<<ENDSCRIPT
<script type="text/javascript">
var new ImagePicker();
window.addEvent('domready', function()
{
	new ImagePicker().setMode('$mode', '$field', '$preview');
});
</script>
ENDSCRIPT;

$gallery = new ImageGallery();
if ($gallery_id) 
{
	$gallery->load($gallery_id);
	$images = $gallery->Images();
}
else
{
	$images = array();
}

$view = new ImageListView($images, "javascript:new ImagePicker().selectImage({image_id}, '{title}')");

?>
<div style="border: solid 1px #666; background: url(/fakoli/images/table-header.png); width: 95%;margin-left: 10px; padding: 2px;">
<div style="float: left; width: auto"><strong>Gallery </strong>
 <select name="gallery_id" id="gallery_id" onchange="new ImagePicker().showImages(this.value)">
<?
	foreach($galleries as $gallery)
	{
		option($gallery->gallery_id, $gallery->gallery_name, $gallery_id);
	} 
?>
 </select>
</div>
<div style="float: right;width: auto"><button class="button" onclick="new ImagePicker().uploadImage()"<?if (!$gallery_id) echo " disabled='disabled'"?>>Upload Image...</button></div>
<div style="clear:both"></div>
</div>
<div id="image_list" style="overflow:auto; width:95%; height: 200px;border: solid 1px #666; margin-left: 10px; padding: 2px; background-color: #fff">
<?
$view->drawGrid();
?>
</div>
<table border="0" style="width: 100%" class="layout">
 <tr>
  <td style="width: 50%"><div style="display: block; float: right; margin: 10px"><div id="preview"></div></div></td>
  <td style="width: 50%">
  <table class="layout">
  
<?
if ($mode != "select")
{
?>
      <tr>
     <td><label for="alignment">Alignment:</label></td>
     <td>
      <select id="alignment" name="alignment" style="width: 120px">
       <option value="">Inline with Text</option>
       <option value="left">Left</option>
       <option value="center">Center</option>
       <option value="right">Right</option>
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="width">Width:</label></td>
     <td><input type="text" style="width: 60px" id="width" name="width" value="" onkeypress="return new ImagePicker().maskInput(event)" onkeyup="new ImagePicker().changeWidth()"/> pixels</td>
    </tr>
    <tr>
     <td><label for="height">Height:</label></td>
     <td><input type="text" style="width: 60px" id="height" name="height" value="" onkeypress="return new ImagePicker().maskInput(event)" onkeyup="new ImagePicker().changeHeight()"/> pixels</td>
    </tr>
    <tr>
     <td colspan="2"><input type="checkbox" id="aspect" name="aspect" value="1" checked />&nbsp;<label for="aspect">Maintain aspect ratio</label></td>
    </tr>

    <tr>
     <td colspan="2" style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Insert Image" onclick="new ImagePicker().insertImage()"/>
     </td>
    </tr>
<?
}
else
{
?>
    <tr>
     <td style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Select Image" onclick="new ImagePicker().setSelectedImage('<?php echo $field ?>')"/>
     </td>
     <td style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Clear Image" onclick="new ImagePicker().clearImage()"/>
     </td>
    </tr>
<?
}
?>    
 </table>
  </td>
 </tr>
</table>