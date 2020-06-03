-- Component Schema for sharing
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.1

CREATE TABLE share_token
(
	share_id		int(10) not null auto_increment,
	token			varchar(40) not null,
	item_type		varchar(100) not null,
	item_key		int(10) not null default 0,
	active			tinyint(3) not null default 0,
	user_id			int(10) not null,
	created_date	datetime,
	PRIMARY KEY(share_id)
);

CREATE TABLE share_access
(
	access_id			int(10) not null auto_increment,
	share_id			int(10) not null default 0,
	user_id				int(10) not null default 0,
	ip_address			varchar(20) not null default '',
	access_timestamp	datetime,
	PRIMARY KEY(access_id)
);

ALTER TABLE share_token ADD INDEX token_idx (token ASC);
ALTER TABLE share_access ADD INDEX share_idx (share_id ASC);

-- END Version 1.1

-- START Version 1.2

ALTER TABLE share_token ADD COLUMN last_updated DATETIME;
ALTER TABLE share_token ADD COLUMN last_updated_by_id INT(10) NOT NULL DEFAULT 0;

-- END Version 1.2