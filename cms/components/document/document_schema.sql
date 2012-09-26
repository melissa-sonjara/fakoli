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

--
-- START Version 1.1
--

CREATE TABLE `document_download` (
  `download_id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `document_id` INTEGER UNSIGNED NOT NULL,
  `download_date` DATETIME NOT NULL,
  `user_id` INTEGER UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`download_id`),
  INDEX `downloads_by_document`(`document_id`)
)
ENGINE = InnoDB;

create or replace view `document_download_daily_stats` as
select
d.document_id as document_id,
l.document_library_id as document_library_id,
date_format(d.download_date,'%Y-%m-%d') as download_date,
count(1) as download_count
from document_download d, document o, document_library l
where d.document_id=o.document_id and l.document_library_id=o.document_library_id
group by d.document_id, date_format(d.download_date,'%Y-%m-%d');

create or replace view `document_download_total_stats` as
select
d.document_id as document_id,
l.document_library_id as document_library_id,
count(1) as download_count
from document_download d, document o, document_library l
where d.document_id=o.document_id and l.document_library_id=o.document_library_id
group by d.document_id;

--
-- END Version 1.1
--

-- START Version 1.2

ALTER TABLE `document` ADD COLUMN `author` VARCHAR(200) NOT NULL DEFAULT '' AFTER `keywords`;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE `document` ADD COLUMN `publication` VARCHAR(200) AFTER `author`;

-- END Version 1.3