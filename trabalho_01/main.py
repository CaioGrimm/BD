import psycopg2

def connect():
    try:
        # Tentando conectar ao PostgreSQL
        conn = psycopg2.connect(
            dbname="BD", 
            user="postgres", 
            password="Qaz123Wsx@", 
            host="localhost", 
            port="5432"
        )
        print("Conexão bem-sucedida!")
        return conn
    except Exception as error:
        print(f"Erro ao conectar ao banco de dados: {error}")
        return None
    
def criar_tabelas(conn):
    with conn.cursor() as cur:
        cur.execute("""
        CREATE TABLE IF NOT EXISTS Produto (
            id SERIAL PRIMARY KEY,
            asin VARCHAR(20) UNIQUE NOT NULL,
            titulo VARCHAR(255),
            grupo VARCHAR(50),
            salesrank INT
        );
        """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS Categoria (
            id SERIAL PRIMARY KEY,
            nome VARCHAR(255)
        );
        """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS Produto_Categoria (
            produto_id INT REFERENCES Produto(id),
            categoria_id INT REFERENCES Categoria(id),
            PRIMARY KEY (produto_id, categoria_id)
        );
        """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS Avaliacao (
            id SERIAL PRIMARY KEY,
            produto_id INT REFERENCES Produto(id),
            cliente_id VARCHAR(20),
            data DATE,
            avaliacao INT,
            votos INT,
            util INT
        );
        """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS Produto_Similar (
            produto_id INT REFERENCES Produto(id),
            similar_id VARCHAR(20),
            PRIMARY KEY (produto_id, similar_id)
        );
        """)
        conn.commit()


# Testando a conexão
conn = connect()
if conn:
    # Aqui você pode executar queries se a conexão for bem-sucedida
    criar_tabelas(conn)
    conn.close()  # Não esqueça de fechar a conexão quando terminar
else:
    print("Conexão falhou.")
