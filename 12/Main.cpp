#include <Siv3D.hpp>

class Spark
{
private:
	Vec2	m_Position;	//座標

	Vec2	m_Normal;	//移動法線
	double	m_Speed;	//移動速度

	bool	m_Destroy;	//消去フラグ

public:
	Spark(const Vec2 & position)
		: m_Position(position), m_Normal(RandomVec2()), m_Speed(Random(20.0)), m_Destroy(false)
	{ }

	//更新
	void Update()
	{
		//移動速度を減速し、停止したら消去フラグを立てる
		if (--m_Speed <= 0.0) m_Destroy = true;
		//法線と速度で座標移動
		m_Position += m_Normal * m_Speed;
	}

	//円を描画
	void Draw()
	{
		Circle(m_Position, 2.0).draw(Palette::Yellow);
	}

	//消去フラグを返す
	bool isDestroy() const
	{
		return m_Destroy;
	}
};

class SparkManager
{
private:
	Array<Spark> m_Spark;

public:
	void Add(const Vec2 & position)
	{
		m_Spark.emplace_back(position);
	}

	void Update()
	{
		Array<Spark>::iterator it = m_Spark.begin();
		Array<Spark>::iterator it_End = m_Spark.end();

		while (it != it_End)
		{
			if (it->isDestroy())
			{
				it = m_Spark.erase(it);
				it_End = m_Spark.end();
			}
			else ++it;
		}

		for (auto & i : m_Spark) i.Update();
	}

	void Draw()
	{
		for (auto & i : m_Spark) i.Draw();
	}
};

class Cutter
{
private:
	const Vec2	m_Size;		//サイズ

	Vec2		m_Position;	//座標
	double		m_Speed;	//移動速度

	bool		m_IsCut;	//切断中であるか

public:
	Cutter(const double & height) : m_Size(0.0, height)
	{
		this->Reset();
	};

	//カッターの状態を初期化
	void Reset(const int score = 0)
	{
		m_Position.set(0.0, 50.0);
		m_Speed = Min(score + 1, 15);

		m_IsCut = false;
	}

	//切断位置決定
	void Cut()
	{
		m_IsCut = true;
	}

	//更新
	void Update()
	{
		//切断フラグが有効であれば、カッターを下に移動する
		if (m_IsCut) m_Position.y += 2.0;
		else
		{
			//カッターを横に移動する
			m_Position.x += m_Speed;

			//画面外に出たらやり直し
			if (Window::Width() < m_Position.x) m_Position.x -= Window::Width();
		}
	}

	//描画
	void Draw()
	{
		//反対側のカッター座標
		Vec2 mirrorPos(Window::Width() - m_Position.x, m_Position.y);

		//線を描画
		Line(m_Position, m_Position + m_Size).draw(5.0);
		Line(mirrorPos, mirrorPos + m_Size).draw(5.0);
	}

	//反射していない側のX座標を取得
	Vec2 position() const
	{
		return Vec2(Min(m_Position.x, Window::Width() - m_Position.x), m_Position.y);
	}

	//反射側のX座標を取得
	Vec2 position_Mirror() const
	{
		return Vec2(Max(m_Position.x, Window::Width() - m_Position.x), m_Position.y);
	}

	Vec2 size() const
	{
		return m_Size;
	}

	//２つのカッターの距離
	double length() const
	{
		return Abs(Window::Width() - m_Position.x * 2.0);
	}
};

class Blick
{
private:
	const Vec2		m_Size;			//サイズ
	const Vec2		m_Position;		//座標

	const Texture	m_Texture;		//画像

	RectF			m_Hide;			//隠す領域

	bool			m_IsDivision;	//分割している状態であるか

public:
	Blick(const double & height)
		: m_Size(Window::Width(), height), m_Position(0.0, Window::Center().y - height / 2.0)
		, m_Texture(L"Example/Brick.jpg"), m_IsDivision(false)
	{}

	//指定範囲を切断する
	TextureRegion SetDivision(const double & x, const double & width)
	{
		m_Hide.set(x, m_Position.y, width, m_Size.y);
		m_IsDivision = true;

		return m_Texture(x, 0.0, width, m_Size.y);
	}

	//初期化
	void Reset()
	{
		m_IsDivision = false;
	}

	//描画
	void Draw()
	{
		//分割状態であれば
		if (m_IsDivision)
		{
			//画像の描画（少し透明にする）
			m_Texture(0.0, 0.0, m_Size).draw(m_Position, Alpha(128));
			//分割された部分を黒四角形で重ねて隠す
			m_Hide.draw(Palette::Black);
		}
		//画像の描画
		else m_Texture(0.0, 0.0, m_Size).draw(m_Position);
	}

	//X座標と幅を考慮しない当たり判定
	bool intersects(const double & y) const
	{
		return m_Position.y < y && y < m_Position.y + m_Size.y;
	}

	//座標
	const Vec2 position() const
	{
		return m_Position;
	}

	//分割している状態であるか
	bool isDivision() const
	{
		return m_IsDivision;
	}
};

class MoveBlock
{
private:
	TextureRegion	m_Texture;	//画像

	Vec2			m_Position;	//座標
	Vec2			m_Target;	//移動先

	bool			m_Move_X;	//移動管理フラグ
	bool			m_Move_Plus;//移動方向フラグ
	bool			m_GameOver;	//ゲームオーバー

	bool			m_Active;	//動作有効であるか

public:
	MoveBlock()
	{
		this->Reset();
	}

	//レンガからブロックを取り出し、移動開始
	void Start(const TextureRegion & texture, const Vec2 & position, const Vec2 & target)
	{
		m_Texture = texture;
		m_Position = position;
		m_Target = target;

		m_Move_X = true;
		m_Move_Plus = position.x < target.x;

		m_GameOver = false;
		m_Active = true;
	}

	//初期化
	void Reset()
	{
		m_GameOver = false;
		m_Active = false;
	}

	//更新
	void Update()
	{
		//動作無効
		if (!m_Active) return;

		if (m_Move_X)
		{
			//右移動
			m_Position.x += m_Move_Plus ? 5.0 : -5.0;

			//目標 X 座標に到達
			if ((m_Move_Plus && m_Target.x <= m_Position.x)
				|| (!m_Move_Plus && m_Position.x <= m_Target.x))
			{
				//補正して下移動に変更
				m_Position.x = m_Target.x;
				m_Move_X = false;
			}

			//画面を超えたら、ゲームオーバー
			if (Window::Width() < m_Position.x + m_Texture.size.x)
			{
				m_Position.x = Window::Width() - m_Texture.size.x;

				m_GameOver = true;
				m_Move_X = false;
			}
		}
		else
		{
			//下移動
			m_Position.y += 5.0;

			//目標 Y 座標に到達したら、動作停止
			if ((!m_GameOver && m_Target.y <= m_Position.y)
				|| (m_GameOver && m_Target.y <= m_Position.y + m_Texture.size.y))
				m_Active = false;
		}
	}

	void Draw()
	{
		//動作無効
		if (!m_Active && !m_GameOver) return;

		//描画
		m_Texture.draw(m_Position);
	}

	TextureRegion texture() const
	{
		return m_Texture;
	}

	Vec2 position() const
	{
		return m_Position;
	}

	bool isActive() const
	{
		return m_Active;
	}

	bool isGameOver() const
	{
		return m_GameOver;
	}
};

class Bar
{
private:
	const double			m_Height;		//画像の高さ

	Array<TextureRegion>	m_TextureArray;	//画像配列
	Vec2					m_Position;		//描画座標

public:
	Bar(const double & height) : m_Height(height)
	{
		this->Reset();
	};

	//画像を末尾に追加
	void Add(const TextureRegion & texture)
	{
		m_TextureArray.emplace_back(texture);
	}

	//初期化
	void Reset()
	{
		m_Position.set(0.0, Window::Height() - m_Height);
		m_TextureArray.clear();
	}

	//更新と描画
	void Update_and_Draw()
	{
		m_Position.x = 0.0;

		for (auto & i : m_TextureArray)
		{
			i.draw(m_Position);
			m_Position.x += i.size.x;
		}
	}

	//次に追加されるブロックの描画座標
	Vec2 target() const
	{
		return m_Position;
	}
};

void Main()
{
	const Font		font_Info(10);	//文字描画用
	const Font		font_Score(25);	//スコア描画用

	int				score = 0;		//スコア

	SparkManager	spark;			//火花
	Cutter			cutter(100.0);	//カッター

	Blick			blick(100.0);	//レンガブロック

	MoveBlock		moveBlock;		//切断ブロック
	Bar				bar(100.0);		//切断ブロック配列

	bool			isHit = false;	//一度当たっているか

	//-------------------------------------------------------//

	//タイトルの変更
	Window::SetTitle(L"ブロックを生み出したら負け");

	//画面サイズを変更（互換性があるのは幅のみ）
	Window::Resize(480, 480);

	//背景色を変更
	Graphics::SetBackground(Palette::Black);

	while (System::Update())
	{
		//何かキーが押されたら、切断開始する
		if (Input::AnyKeyClicked()) cutter.Cut();

		//更新
		moveBlock.Update();
		cutter.Update();
		spark.Update();

		//ブロックが切断済みであれば
		if (blick.isDivision())
		{
			//ブロックの移動が完了したら
			if (!moveBlock.isActive())
			{
				//ゲームオーバーなら
				if (moveBlock.isGameOver())
				{
					//接触部から火花を10個生成
					for (int i = 0; i < 10; ++i)
						spark.Add(Vec2(Random(moveBlock.position().x, bar.target().x), bar.target().y));

					//キーを押してリセット
					if (Input::AnyKeyClicked())
					{
						//初期化
						cutter.Reset();
						blick.Reset();
						moveBlock.Reset();
						bar.Reset();

						score = 0;
						isHit = false;
					}
				}
				else
				{
					//切断した部分を配列に追加する
					bar.Add(moveBlock.texture());

					//スコアを加算
					++score;

					//初期化
					cutter.Reset(score);
					blick.Reset();
					isHit = false;
				}
			}
		}
		else
		{
			//カッターの先端がレンガブロックの領域内であれば
			if (blick.intersects(cutter.position().y + cutter.size().y))
			{
				//一度当たった
				isHit = true;

				//カッターの先端から火花を10個生成
				for (int i = 0; i < 10; ++i)
				{
					spark.Add(cutter.position() + cutter.size());
					spark.Add(cutter.position_Mirror() + cutter.size());
				}
			}
			//一度当たっていて外れた場合は、切断が完了したので移動開始
			else if (isHit)
			{
				moveBlock.Start(
					blick.SetDivision(cutter.position().x, cutter.length())
					, Vec2(cutter.position().x, blick.position().y)
					, bar.target());
			}
		}

		//描画
		blick.Draw();
		moveBlock.Draw();
		bar.Update_and_Draw();
		spark.Draw();
		cutter.Draw();

		//文字列
		font_Info(L"中心を狙って「ブロックの生産を最小限に抑える」謎ゲームです（笑）\nキーボードでカッターの位置を決定します。").draw();
		font_Score((moveBlock.isGameOver() ? L"GAMEOVER" : Format(cutter.length())) + L"\nScore : " + Format(score)).drawCenter(Window::Center());
	}
}