create table overview_params (
    key         text primary key,
    category    integer not null references categories(category) on delete cascade on update cascade,
    statuses    text[]
);


