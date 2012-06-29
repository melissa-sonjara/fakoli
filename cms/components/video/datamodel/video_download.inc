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

class VideoDownload extends DataItem
{
	var $fields = array("download_id"	=>	Number,
						"video_id"	=>	Number,
						"download_date"	=>	Timestamp,
						"user_id"		=>	Number);
	
						
	// Relations
	
	var $relations = array(	"Video"	=>	Video,
							"User"		=>	"");

	
	function Video()
	{
		return $this->getRelated(Video);
	}
	
	function User()
	{
		if (!$this->user_id) return null;
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}

	function VideoDownload()
	{
		$this->primary_key = "download_id";
		$this->table = "video_download";
		
		$this->DataItem(func_get_args());
	}
}

// view
class VideoDownloadDailyStats extends DataItem
{
	// Fields
	
	var $fields = array("video_id"			=>	Number,
						"video_gallery_id"	=>	Number,
						"download_date"			=>	Date,
						"download_count"		=>	Number);

	// Relations
	
	var $relations = array("VideoGallery"	=>	VideoGallery,
						"Video"				=>	Video,
			);
	
	function Video()
	{
		return $this->getRelated(Video);
	}
	
	function VideoGallery()
	{
		return $this->getRelated(VideoGallery);
	}
	
	function VideoDownloadDailyStats()
	{
		$this->table = "video_download_daily_stats";
		
		$this->DataItem(func_get_args());
	}	
}

// view
class VideoDownloadTotalStats extends DataItem
{
	// Fields
	
	var $fields = array("video_id"			=>	Number,
						"video_gallery_id"	=>	Number,
						"download_count"		=>	Number);

	// Relations
	
	var $relations = array("VideoGallery"	=>	VideoGallery);
	
	function VideoGallery()
	{
		return $this->getRelated(VideoGallery);
	}
	
	function VideoDownloadTotalStats()
	{
		$this->table = "video_download_total_stats";
		
		$this->DataItem(func_get_args());
	}	
}
	
?>