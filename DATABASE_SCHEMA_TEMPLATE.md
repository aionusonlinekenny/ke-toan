# DATABASE SCHEMA TEMPLATE
# Điền thông tin về các tables từ Access database của bạn
# Sau khi điền xong, tôi sẽ generate code C++ tương ứng

## KHO_DATA.ACCDB

### Table 1: [Tên Table - VD: HangHoa]
**Description**: Mô tả ngắn về table này

**Columns**:
| Column Name | Data Type | Size | Key | Nullable | Description |
|-------------|-----------|------|-----|----------|-------------|
| MaHang      | Text      | 20   | PK  | No       | Mã hàng hóa |
| TenHang     | Text      | 200  |     | No       | Tên hàng    |
| DonViTinh   | Text      | 50   |     | Yes      | Đơn vị      |
| GiaMua      | Currency  |      |     | Yes      | Giá mua     |
| GiaBan      | Currency  |      |     | Yes      | Giá bán     |

**Relationships**:
- Foreign Key: [Column] -> [Other Table].[Column]
- Example: MaHang -> ChiTietPhieuNhap.MaHang

---

### Table 2: [Tên Table - VD: PhieuNhap]
**Description**:

**Columns**:
| Column Name | Data Type | Size | Key | Nullable | Description |
|-------------|-----------|------|-----|----------|-------------|
|             |           |      |     |          |             |

**Relationships**:
-

---

### Table 3: [...]
...

---

## KETNOI_KETOAN.ACCDB

### Table 1: [Tên Table - VD: TaiKhoan]
**Description**:

**Columns**:
| Column Name | Data Type | Size | Key | Nullable | Description |
|-------------|-----------|------|-----|----------|-------------|
|             |           |      |     |          |             |

**Relationships**:
-

---

## QUERIES (Saved Queries)

### Query 1: [Tên Query]
**SQL**:
```sql
SELECT ... FROM ... WHERE ...
```

---

## FORMS (Main Forms)

### Form 1: [Tên Form]
**Purpose**:
**Data Source**: [Table/Query]
**Controls**: [List main controls]

---

## REPORTS (Main Reports)

### Report 1: [Tên Report]
**Purpose**:
**Data Source**: [Table/Query]
**Grouping**:

---

## NOTES
- Các thông tin bổ sung về business logic
- VBA modules quan trọng
- Validation rules
- Default values
