-- START Version 1.1

CREATE TABLE color_palette
(
	palette_id		int(10) not null auto_increment,
	name			varchar(100) not null,
	background		varchar(20) null,
	stroke			varchar(20) null,
	button			varchar(20) null,
	data_series_1	varchar(20) null,
	data_series_2	varchar(20) null,
	data_series_3	varchar(20) null,
	data_series_4	varchar(20) null,
	data_series_5	varchar(20) null,
	data_series_6	varchar(20) null,
	data_series_7	varchar(20) null,
	data_series_8	varchar(20) null,
	data_series_9	varchar(20) null,
	data_series_10	varchar(20) null,
	data_series_11	varchar(20) null,
	data_series_12	varchar(20) null,
	data_series_13	varchar(20) null,
	data_series_14	varchar(20) null,
	data_series_15	varchar(20) null,
	PRIMARY KEY(palette_id)
);

-- END Version 1.1