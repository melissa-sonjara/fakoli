-- Component Schema for link_library
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.1

CREATE TABLE `link_record`
(
	`link_id`			int(10) unsigned NOT NULL AUTO_INCREMENT,
  	`title` 			varchar(1000) NOT NULL,
  	`url`				varchar(1000) NOT NULL,
	`description`		text null,
	`published`			tinyint(3) not null default 0,
	`last_modified` 	datetime,
	`link_library_id`	int(10) unsigned NOT NULL DEFAULT 0,
	`owner_id`			int(10) unsigned NOT NULL DEFAULT 0,
	PRIMARY KEY(`link_id`)
) ENGINE=InnoDB;


CREATE TABLE `link_library`
(
	`link_library_id`	int(10) unsigned NOT NULL AUTO_INCREMENT,
	`name`				varchar(1000) NOT NULL,
	`identifier`		varchar(1000) NOT NULL,
	`description`		text null,
	`owner_id`			int(10) unsigned NOT NULL DEFAULT 0,
	`read_access`		varchar(1000) NULL,
	`write_access`		varchar(1000) NULL,
	`create_date`		datetime null,
	`last_modified`		datetime null,
	PRIMARY KEY(`link_library_id`)
) ENGINE=InnoDB;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `link_library` ADD COLUMN `enabled` tinyint(3) not null default 0;

-- END Version 1.2