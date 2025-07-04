/**
 * @brief �t���X�N���[���N���b�h�`��p�̒��_�V�F�[�_�[�o�̓f�[�^�\����
 *
 * �|�X�g�v���Z�X��2D�`��Ȃǂŉ�ʑS�̂ɃG�t�F�N�g��������ۂɎg�p�����A
 * �t���X�N���[���N���b�h�̊e���_����s�N�Z���V�F�[�_�[�֓n���f�[�^���`���܂��B
 */
struct VS_OUT
{
    /**
     * @brief �N���b�s���O��ԏ�̒��_���W
     *
     * SV_POSITION�Z�}���e�B�N�X�������A���X�^���C�U��s�N�Z���V�F�[�_�[�Ŏg�p����܂��B
     */
    float4 position : SV_POSITION;

    /**
     * @brief �e�N�X�`�����W�iUV���W�j
     *
     * 0�`1�͈̔͂ŁA�e�N�X�`���T���v�����O���ʏ�̈ʒu����ɗ��p����܂��B
     */
    float2 texcoord : TEXCOORD;
};

/**
 * @brief ���ԏ����V�F�[�_�[�ɓn���萔�o�b�t�@
 *
 * �A�j���[�V�����⎞�Ԍo�߂ɂ��G�t�F�N�g����̂��߁A
 * �A�v���P�[�V���������猻�݂̌o�ߎ��ԁi�b�j��n���܂��B
 */
cbuffer TimeCBuffer : register(b10)
{
    /**
     * @brief �o�ߎ��ԁi�b�j
     *
     * �V�F�[�_�[���ŃA�j���[�V�����⎞�Ԑ���ɗ��p����܂��B
     */
    float iTime;
    float signalTime; // �T�C���g�̎��ԕω�

    /**
     * @brief �\���̃p�f�B���O�p�ϐ�
     *
     * �������A���C�����g�⏫���̊g���̂��߂Ɋm�ۂ���Ă��܂��B
     */
    float3 pud0;
}

cbuffer ScreenSizeCBuffer : register(b11)
{
    /**
     * @brief ��ʃT�C�Y�i�s�N�Z���P�ʁj
     *
     * �V�F�[�_�[���ŉ�ʂ̉𑜓x�Ɋ�Â��v�Z���s�����߂Ɏg�p����܂��B
     */
    float2 iResolution;
    /**
     * @brief �\���̃p�f�B���O�p�ϐ�
     *
     * �������A���C�����g�⏫���̊g���̂��߂Ɋm�ۂ���Ă��܂��B
     */
    float2 pud1;
};