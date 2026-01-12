# GROCERYFLOW IMS System Flowchart

This flowchart illustrates the system architecture and workflow of GROCERYFLOW IMS, showing the main application flow from login through role-based access to various modules.

```mermaid
flowchart TB
  A[GROCERYFLOW IMS] --> B[Register / Login]
  B --> C{Database Connected?}
  C -- No --> E[Show error and retry<br>(MySQL must be running)]
  C -- Yes --> R{Role}

  R --> ADM[Admin]
  R --> STF[Staff]

  subgraph "Admin Panel"
    direction TB
    ADM --> AD1[Dashboard]

    AD1 --> INV_A[Inventory Management]
    INV_A --> A_ADD[Add Product]
    INV_A --> A_UPD[Update Product]
    INV_A --> A_VIEW[View Inventory]
    INV_A --> A_CAT[Categorize Products]

    AD1 --> SALES_A[Sales Processing]
    SALES_A --> A_REC[Record Sale]
    SALES_A --> A_STK[Update Stock]
    SALES_A --> A_PROF[Calculate Profit]
    SALES_A --> A_TXN[Transaction History]

    AD1 --> RPT_A[Reports]
    RPT_A --> A_GEN[Generate Sales Reports]
    RPT_A --> A_SUM[Profit Summary]

    AD1 --> ACC_A[Account]
    ACC_A --> A_UPRO[Update Profile]
  end

  subgraph "Staff Panel"
    direction TB
    STF --> SD1[Dashboard]

    SD1 --> INV_S[Inventory Management]
    INV_S --> S_ADD[Add Product]
    INV_S --> S_UPD[Update Product]
    INV_S --> S_VIEW[View Inventory]
    INV_S --> S_CAT[Categorize Products]

    SD1 --> SALES_S[Sales Processing]
    SALES_S --> S_REC[Record Sale]
    SALES_S --> S_STK[Update Stock]
    SALES_S --> S_PROF[Calculate Profit]
    SALES_S --> S_TXN[Transaction History]

    SD1 --> RPT_S[Reports]
    RPT_S --> S_GEN[Generate Sales Reports]

    SD1 --> ACC_S[Account]
    ACC_S --> S_UPRO[Update Profile]
  end

  A_TXN --> END[(End)]
  S_TXN --> END
```

## Key Components

- **Application Entry**: GROCERYFLOW IMS starts with registration/login
- **Database Check**: Validates MySQL connection before proceeding
- **Role-Based Access**: Separate panels for Admin and Staff users
- **Modules**:
  - Inventory Management: Product operations and categorization
  - Sales Processing: Recording sales, stock updates, profit calculation
  - Reports: Sales reports and profit summaries
  - Account: User profile management

> **Note**: This diagram is rendered natively by GitHub using Mermaid. No external rendering services are used.
