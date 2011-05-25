<?php


class AttachmentManager
{
	function AttachmentManager()
	{
		
	}
	
	static function deleteAttachment($attachment)
	{
		if(!$attachment)
			return;
			
		global $config;
		
		$dir = $config["attachmentdir"];
		$base = $config['uploadbase'];
			
		$file = "$base/$dir{$attachment->local_filename}";

		if (file_exists($file))
		{
			unlink($file);
		}
		$attachment->delete();		
	}
}