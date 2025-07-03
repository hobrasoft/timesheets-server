begin;

insert into version (version) values (11);

create or replace function attendance.easter_date(year integer)
returns date as $$
declare
    a integer := year % 19;
    b integer := floor(year / 100);
    c integer := year % 100;
    d integer := floor(b / 4);
    e integer := b % 4;
    f integer := floor((b + 8) / 25);
    g integer := floor((b - f + 1) / 3);
    h integer := (19 * a + b - d - g + 15) % 30;
    i integer := floor(c / 4);
    k integer := c % 4;
    l integer := (32 + 2*e + 2*i - h - k) % 7;
    m integer := floor((a + 11*h + 22*l) / 451);
    month integer := floor((h + l - 7*m + 114) / 31);
    day integer := ((h + l - 7*m + 114) % 31) + 1;
begin
    return make_date(year, month, day);
end;
$$ language plpgsql immutable;

create or replace function attendance.copy_holidays(from_year integer, to_year integer)
returns void as $$
declare
    easter date := attendance.easter_date(to_year);
    good_friday date := easter - interval '2 days';
    easter_monday date := easter + interval '1 day';
begin
    -- Smazat svátky cílového roku, pokud už existují
    delete from attendance.holidays where extract(year from date) = to_year;

    -- Zkopírovat všechny ostatní svátky kromě pohyblivých
    insert into attendance.holidays(date, description)
    select 
        make_date(to_year, extract(month from date)::int, extract(day from date)::int) as new_date,
        description
    from attendance.holidays
    where extract(year from date) = from_year
      and description not ilike 'Velký pátek'
      and description not ilike 'Velikonoce';

    -- Přidat nové Velikonoce
    insert into attendance.holidays(date, description) values
        (good_friday::date, 'Velký pátek'),
        (easter_monday::date, 'Velikonoce');
end;
$$ language plpgsql;

commit;
