-- Component Schema for scroll
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0

CREATE TABLE scroll
(
	scroll_id				 int(10) not null auto_increment,
	identifier				 varchar(255) not null,
	title					 varchar(255) not null default '',
	show_internal_navigation tinyint(3) not null default 0,
	meta_tag_description	 text,
	meta_tag_keyword		 varchar(1000),
	site_id					 int(10) not null,
	exclude_from_search		 tinyint(3) not null default 0,
	created_date			 datetime,
	edited_date				 datetime,
	PRIMARY KEY(scroll_id)
) ENGINE=InnoDB;

CREATE TABLE scroll_part
(
	scroll_part_id	int(10) not null auto_increment,
	scroll_id		int(10) not null,
	title			varchar(255) not null default '',
	content_type	varchar(20) not null default 'HTML',
	content			text,
	php_code_file	varchar(1000) not null default '',
	css_class		varchar(255) not null default '',
	sort_order		int(10) not null default 0,
	PRIMARY KEY(scroll_part_id)

) ENGINE=InnoDB;

-- END Version 1.0