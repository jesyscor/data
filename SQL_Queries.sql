SELECT COUNT(*) FROM [GD2C2014].[gd_esquema].Maestra
--
EXEC sp_spaceused N'gd_esquema.Maestra';
--
SELECT *
FROM INFORMATION_SCHEMA.SCHEMATA
WHERE SCHEMA_NAME = 'LOS_DESCONOCIDOS';
--
SELECT * FROM sys.schemas
-----------------------------------------------------------
SELECT Hotel_Recarga_Estrella 
FROM gd_esquema.Maestra
WHERE year(Estadia_Fecha_Inicio) = 2015
--
select top 20 f.Factura_Nro,
  (select count(d.Item_Factura_Cantidad)
    from gd_esquema.Maestra as d
    where f.Factura_Nro=d.Factura_Nro) as cantidad
--  (select sum(d.preciounitario*cantidad)
--   from Detalles as d
--   where f.numero=d.numerofactura) as total
from gd_esquema.Maestra as f;