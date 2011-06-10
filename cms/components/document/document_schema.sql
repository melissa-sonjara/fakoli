-- Fakoli Document Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `document` (
  `document_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(1000) NOT NULL,
  `publication_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `keywords` varchar(1000) DEFAULT NULL,
  `notes` text,
  `public` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `file` varchar(400) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE  `document_folder` (
  `folder_id` int(10) unsigned NOT NULL auto_increment,
  `path` varchar(255) NOT NULL,
  `permissions` varchar(100) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`folder_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `document_library` (
  `document_library_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `manage_folders` varchar(200) NOT NULL DEFAULT '',
  `upload_files` varchar(200) NOT NULL DEFAULT '',
  `delete_files` varchar(200) NOT NULL DEFAULT '',
  `create_date` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `allow_access` varchar(200) NOT NULL DEFAULT '',
  `identifier` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`document_library_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


-- END Version 1.0
