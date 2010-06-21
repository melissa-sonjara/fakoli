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

require_once "../../../include/config.inc";
require_once "../../../cms/datamodel/image.inc";

$useMooTools = 1;

$mode    = $_GET["Mode"];     // "insert" or "select" - "select" is default
$editor  = $_GET["Editor"];   // in "insert" mode - ID of the RTE to insert the image into
$field   = $_GET["Field"];    // in "select" mode - ID of the field to set the value of
$preview = $_GET["Preview"];  // in "select" mode - ID of the element to display the preview thumbnail.

$title = ($mode == "insert") ? "Insert Images" : "Select an Image";

$styles .= <<<ENDSTYLES
<style type="text/css">

#preview
{
border:1px inset #888;
display:table-cell;
height:200px;
text-align:center;
vertical-align:middle;
width:200px;
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

ob_start();

?>
<script type="text/javascript">
/* <![CDATA[ */
<!--

var myLinks;
var searchResults;
var browse;

var selectedImage;
var selectedThumbnail;
var selectedTitle;
var selectedKey = 0;

var originalWidth = 1;
var originalHeight = 1;

function selectImage(image_file, thumbnail, title, image_id)
{
	selectedImage = image_file;
	selectedThumbnail = thumbnail;
	selectedTitle = title;
	selectedKey = image_id;
	
	$('preview').innerHTML = "<img src='" + thumbnail + "' width='150' border='0' alt='" + title + "'/>";
	<? if ($mode != "select") echo "getImageSize();"; ?>
	
}

<?
if ($mode == "select")
{
?>
function insertImage()
{
	if (selectedImage == "") return;
	
	var srcdoc = self.opener.document;
	
	srcdoc.getElementById('<?echo $field?>').value = selectedKey;
	srcdoc.getElementById('<?echo $preview?>').src = selectedThumbnail;
	window.close();
}
<?
}
else
{
?>
function insertImage()
{
    if (selectedImage == "") return;
    
    var align = $('alignment').value;
    var size = $('size').value;
    
    var src = (size == "thumbnail") ? selectedThumbnail : selectedImage;
    
    var a = (align != "" && align != "inline") ? " align=\"" +align + "\"" : "";
    
    var w = $('width').value;
    var h = $('height').value;
    var s = "";
    
    if (w != "" || h != "")
    {
    	s = "style=\"";

    	if (w != "")
    	{
    		s += "width: " + w + "px;";
		}
		
		if (h != "")
		{
			s += "height: " + h + "px;";
		}
		
		s += "\" ";
	}
			
    var img  = "<img image_id=\"" + selectedKey + "\" src=\"" + src + "\" " + s + "border=\"0\"" + " alt=\"" + selectedTitle + "\"" + a + ">";
    
    var editor = self.opener.<?echo $editor ?>;
    editor.insertAtSelection(img);
    window.close();
}

function getImageSize()
{
	var img = ($('size').value == "thumbnail") ? selectedThumbnail : selectedImage;
	
	if (!img) return;
	
	var result = httpRequest(encodeURI("get_image_size.php?img=" + img));
	
	if (!result.match(/^\d+,\d+/))
	{
		$('width').value = "";
		$('height').value = "";
	}
	
	var dimensions = result.split(",");
	$('width').value = dimensions[0];
	$('height').value = dimensions[1];
	originalWidth = dimensions[0];
	originalHeight = dimensions[1];
}

function changeWidth()
{
	if (!$('aspect').checked) return;
	
	var width = $('width').value;
	if (width == "") 
	{
		width = 0;
		$('width').value = "0";
	}	
	
	$('height').value = (width/originalWidth * originalHeight).toFixed(0);
}

function changeHeight()
{
	if (!$('aspect').checked) return;

	var height = $('height').value;
	if (height == "") 
	{
		height = 0;
		$('height').value = "0";
	}	
	
	$('width').value = (height/originalHeight * originalWidth).toFixed(0);

}

function maskInput(e)
{
	var key;
	var keychar;

	if (window.event)
	{
		key = window.event.keyCode;
	}
	else if (e)
	{
		key = e.which;
	}
	else
	{
		return true;
	}

	keychar = String.fromCharCode(key);

	// control keys
	if ((key==null) || (key==0) || (key==8) ||
		(key==9) || (key==13) || (key==27) )
	{
		return true;
	}

	// numbers
	else if ((("0123456789.,").indexOf(keychar) > -1))
	{
		return true;
	}

	return false;
}

<?
}
?>




//-->

/* ]]> */
</script>
<script type="text/javascript">
<!--
function toggleFolder(id, openStyle, closedStyle)
{
	var div = document.getElementById(id + "_contents");
	var link = document.getElementById(id);

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.className = openStyle;
	}
	else
	{
		div.style.display="none";
		link.className = closedStyle;
	}
}

function loadOnDemand(id, url)
{
	var div = document.getElementById(id + "_contents");
	if (!div.loaded)
	{
		html = httpRequest(url);
		div.innerHTML = html;
		div.loaded = true;
	}
}
	
function clearCheckBoxes(id, except)
{
	var div = document.getElementById(id);
	var cboxes = div.getElementsByTagName("input");
	for(i = 0; i < cboxes.length; ++i)
	{
		if (cboxes[i] != except)
		{
			cboxes[i].checked = false;
		}
	}
	
	except.form[id].value = except.value;
}

//-->
</script>

<?

$script .= ob_get_contents();
ob_end_clean();

$images = query(image);

$count = 0;

require_once admin_begin_popup;
?>

<div>
<div style="overflow:auto; width:100%; height: 300px; clear: left; border: 1px; border-style: solid">
	<table class="layout" width="100%"><tr>
 
 	
	<?

	foreach($images as $image)
	{
		?>
		
		<td>
		<a class="searchitem" href="#" onclick="selectImage('<?echo jsSafe($image->image_file) ?>', '<?echo $image->thumbnail?>', '<?echo jsSafe(trim($image->title)) ?>', '<?echo $image->image_id ?>');"><?echo $image->formatThumbnail(120) ?><br/><?echo ellipsis($image->title, 80) ?></a>
	  	<!--   <img src="<?echo $image->thumbnail ?> " alt="Small version of image" width="150" height="69" /> -->
	    </td>
	    <?php 
		++$count;
		if (($count % 3) == 0)
		{
			echo "</tr><tr>";
		}				
    }
    ?>
</tr>	
</table>
</div>
<table border="0" style="width: 100%" class="layout">
 <tr>
  <td style="width: 50%"><div id="preview"></div></td>
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
     <td style="vertical-align: middle"><label for="Size">Image Size:</label></td>
     <td>
      <select id="size" name="size" style="width: 120px" onchange="getImageSize()">
       <option value="thumbnail">Thumbnail</option>
       <option value="image">Full-Size</option>
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="width">Width:</label></td>
     <td><input type="text" style="width: 60px" id="width" name="width" value="" onkeypress="return maskInput(event)" onkeyup="changeWidth()"/> pixels</td>
    </tr>
    <tr>
     <td><label for="height">Height:</label></td>
     <td><input type="text" style="width: 60px" id="height" name="height" value="" onkeypress="return maskInput(event)" onkeyup="changeHeight()"/> pixels</td>
    </tr>
    <tr>
     <td colspan="2"><input type="checkbox" id="aspect" name="aspect" value="1" checked="true">&nbsp;<label for="aspect">Maintain aspect ratio</label></td>
    </tr>

    <tr>
     <td colspan="2" style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Insert Image" onclick="insertImage()"/>
     </td>
    </tr>
<?
}
else
{
?>
    <tr>
     <td style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Select Image" onclick="insertImage()"/>
     </td>
     <td style="text-align: center"><br/>
      <input type="button" class="button" name="insert" value="Clear Image" onclick="selectedKey = 0; selectedThumbnail = '/images/noimage.gif'; insertImage()"/>
     </td>
    </tr>
<?
}
?>    
 </table>
  </td>
 </tr>
</table>
</div>

<?
require_once admin_end_popup;
?>