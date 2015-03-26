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
 
Fakoli::using("video");
Fakoli::usingFeature("data_view");

$videos = Query::create(Video, "ORDER BY title")->execute();

$table = new DataListView($videos, "videos");

$videoPath = Settings::getValue("video", "video_folder");

$table->column("Title", "<a href='#' onclick='new VideoPicker().selectVideo(this, {video_id}, \"{VideoManager::getVideoURI}\", \"/action/video/thumbnail?video_id={video_id}\");'>{title}</a><span class='image_id'>{image_id}</span>", true, "width: 35%")
	  ->column("Resolution", "{width} x {height}", true, "width: 10%")
	  ->column("Owner", "{User.first_name} {User.last_name}", true, "width: 25%")
	  ->column("Last Modified", "{last_modified}", true, "width: 20%");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No videos have been uploaded,";
$table->zebra = false;

$script .= "<style type='text/css'>span.image_id { display: none }</style>\n";
$script .= $table->writeScript();

echo $script;

$table->drawView();
?>
<p>
<label for="insert_mode">Insert as </label><select id="insert_mode">
 <option value="embed">Embedded Player</option>
 <option value="button">Lightbox button</option>
 <option value="link">Lightbox link</option>
 <option value="thumbnail">Thumbnail w/ Lightbox</option>
 <option value="download">Download Link</option>
</select>
&nbsp;&nbsp;&nbsp;&nbsp;<input type="checkbox" class="checkbox" id="transcript" value="1"/>&nbsp;<label for="transcript">Insert Link to Transcript</label>
<br/>
<br/>
<a href="#" class='button' id='insert_video_button' onclick="new VideoPicker().insert()">Insert Video</a>