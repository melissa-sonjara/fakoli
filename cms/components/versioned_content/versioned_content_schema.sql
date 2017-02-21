-- Component Schema for versioned_content
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0

CREATE TABLE content_version
(
	content_version_id	int(10) not null auto_increment,
	content_class		varchar(255) not null,
	content_id			int(10) not null,
	version_number		int(10) not null,
	content				longtext,
	last_modified		datetime,
	last_modified_by	int(10) not null default 0,
	approved			tinyint(3) not null default 0,
	approved_date		datetime,
	approved_by			int(10) not null default 0,
	PRIMARY KEY(content_version_id)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

ALTER TABLE content_version ADD INDEX content_version_idx (content_class, content_id);

-- END Version 1.0